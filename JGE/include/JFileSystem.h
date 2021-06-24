#ifndef _J_FILE_SYSTEM_H_
#define _J_FILE_SYSTEM_H_

#include "zfsystem.h"

namespace zfs = zip_file_system;

#include "unzip/unzip.h"

//////////////////////////////////////////////////////////////////////////
/// Interface for low level file access with ZIP archive support. All
/// file operations in JGE are handled through this class so if a ZIP
/// archive is attached, all the resources will be loaded from the
/// archive file.
///
//////////////////////////////////////////////////////////////////////////

class JZipCache {
public:
    JZipCache();
    ~JZipCache();
    std::map<std::string, zfs::filesystem::limited_file_info> dir;
};

class JFileSystem {
private:
    std::string mSystemFSPath, mUserFSPath;
    zfs::filesystem *mSystemFS, *mUserFS;
    static JFileSystem* mInstance;
    zfs::izfstream mFile;

    std::map<std::string, JZipCache*> mZipCache;
    unsigned int mZipCachedElementsCount;
    std::string mZipFileName;
    int mFileSize;
    char* mPassword;
    bool mZipAvailable;
    void preloadZip(const std::string& filename);
    zfs::izfstream mZipFile;
    zfs::filesystem::limited_file_info* mCurrentFileInZip;

    std::vector<std::string>& scanRealFolder(const std::string& folderName, std::vector<std::string>& results);

public:
    inline zfs::izfstream& current_file() noexcept { return mFile; }

    //////////////////////////////////////////////////////////////////////////
    /// Attach ZIP archive to the file system.
    ///
    /// @param zipfile - Name of ZIP archive.
    /// @param password - Password for the ZIP archive. Default is NULL.
    ///
    /// @return Status of the attach operation.
    ///
    //////////////////////////////////////////////////////////////////////////
    bool AttachZipFile(const std::string& zipfile, char* password = NULL);

    //////////////////////////////////////////////////////////////////////////
    /// Release the attached ZIP archive.
    ///
    //////////////////////////////////////////////////////////////////////////
    void DetachZipFile();

    // Manually Clear the zip cache
    void clearZipCache();

    //////////////////////////////////////////////////////////////////////////
    /// Get the singleton instance
    ///
    //////////////////////////////////////////////////////////////////////////
    static JFileSystem* GetInstance();

    static void Destroy();

    // closes temporary files when the machine goes to sleep
    void Pause();
    //////////////////////////////////////////////////////////////////////////
    /// Open file for reading.
    ///
    //////////////////////////////////////////////////////////////////////////
    bool OpenFile(const std::string& filename);

    // Fills the vector results with a list of children of the given folder
    std::vector<std::string>& scanfolder(const std::string& folderName, std::vector<std::string>& results);
    std::vector<std::string> scanfolder(const std::string& folderName);
    //////////////////////////////////////////////////////////////////////////
    /// Read data from file.
    ///
    /// @param buffer - Buffer for reading.
    /// @param size - Number of bytes to read.
    ///
    /// @return Number of bytes read.
    ///
    //////////////////////////////////////////////////////////////////////////
    int ReadFile(void* buffer, int size);

    //////////////////////////////////////////////////////////////////////////
    /// Get size of file.
    ///
    //////////////////////////////////////////////////////////////////////////
    int GetFileSize();
    int GetFileSize(zfs::izfstream& file);

    //////////////////////////////////////////////////////////////////////////
    /// Close file.
    ///
    //////////////////////////////////////////////////////////////////////////
    void CloseFile();

    //////////////////////////////////////////////////////////////////////////
    /// Set root for all the following file operations
    ///
    /// @resourceRoot - New root.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetSystemRoot(const std::string& resourceRoot);
    std::string GetSystemRoot() { return mSystemFSPath; };

    void SetUSerRoot(const std::string& resourceRoot);
    std::string GetUserRoot() { return mUserFSPath; };

    bool openForRead(zfs::izfstream& File, const std::string& FilePath);
    bool readIntoString(const std::string& FilePath, std::string& target);
    bool openForWrite(std::ofstream& File, const std::string& FilePath, std::ios_base::openmode mode = std::ios_base::out);
    bool Rename(std::string from, std::string to);

    // Returns true if strFilename exists somewhere in the fileSystem
    bool FileExists(const std::string& strFilename);

    // Returns true if strdirname exists somewhere in the fileSystem, and is a directory
    bool DirExists(const std::string& strDirname);

    bool MakeDir(const std::string& dir);

    static void init(const std::string& userPath, const std::string& systemPath = "");

    // AVOID Using This function!!!
    /*
    This function is deprecated, but some code is still using it
    It used to give a pathname to a file in the file system.
    Now with the support of zip resources, a pathname does not make sense anymore
    However some of our code still relies on "physical" files not being in zip.
    So this call is now super heavy: it checks where the file is, and if it's in a zip, it extracts
    it to the user Filesystem, assuming that whoever called this needs to access the file through its pathname later on
    */
    std::string GetResourceFile(std::string filename);

protected:
    JFileSystem(const std::string& userPath, const std::string& systemPath = "");
    ~JFileSystem();
};

#endif
