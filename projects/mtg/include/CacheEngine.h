#include "PrecompiledHeader.h"

#include <wge/thread.hpp>
#include <wge/log.hpp>

#include <queue>
#include <set>
#include <chrono>

struct CacheRequest {
    CacheRequest() {}

    CacheRequest(std::string inFilename, int inSubmode, int inCacheID)
        : filename(inFilename), submode(inSubmode), cacheID(inCacheID) {}

    std::string filename;
    int submode;
    int cacheID;
};

const std::chrono::milliseconds kIdleTime(100);

class CardRetrieverBase {
public:
    CardRetrieverBase(WCache<WCachedTexture, JTexture>& inCache) : mTextureCache(inCache) {}

    virtual ~CardRetrieverBase() {}

    virtual void QueueRequest(const std::string& inFilePath, int inSubmode, int inCacheID) = 0;

protected:
    WCache<WCachedTexture, JTexture>& mTextureCache;
};

/*
**
*/
class UnthreadedCardRetriever : public CardRetrieverBase {
public:
    UnthreadedCardRetriever(WCache<WCachedTexture, JTexture>& inCache) : CardRetrieverBase(inCache) {
        WGE_LOG_TRACE("Unthreaded version");
    }

    virtual ~UnthreadedCardRetriever() {}

    /*
    **  In a non-threaded model, simply pass on the request to the texture cache directly
    */
    void QueueRequest(const std::string& inFilePath, int inSubmode, int inCacheID) {
        mTextureCache.LoadIntoCache(inCacheID, inFilePath, inSubmode);
    }
};

/**
** Threaded implementation.
*/
class ThreadedCardRetriever : public CardRetrieverBase {
public:
    ThreadedCardRetriever(WCache<WCachedTexture, JTexture>& inCache) : CardRetrieverBase(inCache), mProcessing(true) {
        WGE_LOG_TRACE("Threaded Version");
        mWorkerThread = wge::thread(ThreadProc, this);
    }

    virtual ~ThreadedCardRetriever() {
        WGE_LOG_TRACE("Tearing down ThreadedCardRetriever");
        mProcessing = false;
        mWorkerThread.join();
    }

    void QueueRequest(const std::string& inFilePath, int inSubmode, int inCacheID) {
        std::lock_guard<wge::mutex> lock(mMutex);
        // mRequestLookup is used to prevent duplicate requests for the same id
        if (mRequestLookup.find(inCacheID) == mRequestLookup.end() &&
            mTextureCache.cache.find(inCacheID) == mTextureCache.cache.end()) {
            WGE_LOG_TRACE("Queueing request: {}", inFilePath);

            mRequestLookup.insert(inCacheID);
            mRequestQueue.push(CacheRequest(inFilePath, inSubmode, inCacheID));

            // capping the number of queued decodes to 7, as this is
            // the maximum # of cards we display concurrently in the deck editor.
            if (mRequestQueue.size() > 7) {
                int cacheIDToRemove;
                while (mRequestQueue.size() > 7) {
                    // pop the older requests out of the queue
                    cacheIDToRemove = mRequestQueue.front().cacheID;
                    mRequestQueue.pop();
                    mRequestLookup.erase(cacheIDToRemove);

                    assert(mRequestLookup.size() - mRequestQueue.size() < 2);
                }
            }
        }
    }

protected:
    ThreadedCardRetriever();

    static void ThreadProc(void* inParam) {
        WGE_LOG_TRACE("Entering");

        ThreadedCardRetriever* instance = reinterpret_cast<ThreadedCardRetriever*>(inParam);
        if (instance) {
            while (instance->mProcessing) {
                while (!instance->mRequestQueue.empty()) {
                    CacheRequest request;
                    {
                        std::lock_guard<wge::mutex> lock(instance->mMutex);
                        request = instance->mRequestQueue.front();
                        instance->mRequestQueue.pop();
                    }

                    instance->mTextureCache.LoadIntoCache(request.cacheID, request.filename, request.submode);

                    {
                        std::lock_guard<wge::mutex> lock(instance->mMutex);
                        instance->mRequestLookup.erase(request.cacheID);
                    }

                    // not sure this is necessary, adding it to potentially prevent SIGHUP on the psp
                    // rumour has it that if a worker thread doesn't allow the main thread a chance to run, it can hang
                    // the unit
#ifdef PSP
                    wge::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
                }

                wge::this_thread::sleep_for(kIdleTime);
            }
        }
    }

    wge::thread mWorkerThread;

    std::queue<CacheRequest> mRequestQueue;
    std::set<int> mRequestLookup;
    wge::mutex mMutex;
    volatile bool mProcessing;
};

class CacheEngine {
public:
    template <class T>
    static void Create(WCache<WCachedTexture, JTexture>& inCache) {
        WGE_LOG_TRACE("Creating Card Retriever instance");
        sInstance = NEW T(inCache);
        ThreadedCardRetriever* test = dynamic_cast<ThreadedCardRetriever*>(sInstance);
        sIsThreaded = (test != NULL);
    }

    static CardRetrieverBase* Instance() { return sInstance; }

    static void Terminate() { SAFE_DELETE(sInstance); }

    static bool IsThreaded() { return sIsThreaded; }

    static CardRetrieverBase* sInstance;
    static bool sIsThreaded;
};

CardRetrieverBase* CacheEngine::sInstance = NULL;
bool CacheEngine::sIsThreaded = false;
