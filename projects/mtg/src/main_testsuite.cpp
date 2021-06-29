#include "GameOptions.h"
#include "MTGDeck.h"
#include "TestSuiteAI.h"

#include <JApp.h>
#include <JGE.h>
#include <JGameLauncher.h>

#include <wge/log.hpp>
#include <wge/time.hpp>
#include <wge/memory.hpp>

class WagicWrapper {
public:
    WagicWrapper();
    virtual ~WagicWrapper();

private:
    JGE* m_engine;
    JApp* m_app;

    wge::unique_ptr<JGameLauncher> m_launcher;
};

bool JGEToggleFullscreen() { return true; }

void JGECreateDefaultBindings() {}

WagicWrapper::WagicWrapper() : m_engine(nullptr), m_app(nullptr), m_launcher(std::make_unique<JGameLauncher>()) {
    JGECreateDefaultBindings();

    m_engine = JGE::GetInstance();
    m_app    = m_launcher->GetGameApp();
    m_app->Create();
    m_engine->SetApp(m_app);
}

WagicWrapper::~WagicWrapper() {
    if (m_engine) {
        m_engine->SetApp(nullptr);
    }

    if (m_app) {
        m_app->Destroy();
        delete m_app;
        m_app = nullptr;
    }

    JGE::Destroy();
    m_engine = nullptr;
}

int main(int argc, char* argv[]) {
    auto wagic_core = std::make_unique<WagicWrapper>();

    MTGCollection()->loadFolder("sets/primitives/");
    MTGCollection()->loadFolder("sets/", "_cards.dat");
    options.reloadProfile();

    TestSuite testSuite("test/_tests.txt");

    const auto start_time = wge::clock::now();
    const auto result     = testSuite.run();
    const auto end_time   = wge::clock::now();
    const auto elapsed    = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    WGE_LOG_INFO("done: failed test: {} out of {} total needed_time: {} seconds", result,
                 testSuite.nbTests + testSuite.nbAITests, elapsed);
    return result;
}
