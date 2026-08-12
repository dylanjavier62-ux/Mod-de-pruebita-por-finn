#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelCompleteLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

using namespace geode::prelude();

// Variables globales para el mod
int g_noclipDeaths = 0;
bool g_noclipEnabled = true;
CCLabelBMFont* g_deathLabel = nullptr;
int g_startPercent = 0;

// 1. Bypass de FPS: Forzamos los FPS deseados (ej. 240 FPS) ignorando la sincronización vertical
class $modify(MyCCScheduler, CCScheduler) {
    void update(float dt) {
        // Puedes cambiar 240.0f por los FPS que desees para tu celular
        float targetFPS = 240.0f;
        
        // Forzamos el límite de tiempo por frame para que vaya a los FPS indicados
        CCScheduler::setAnimationInterval(1.0f / targetFPS);
        
        CCScheduler::update(dt);
    }
};

// 2. Hook en PlayLayer para detectar el StartPos y el porcentaje inicial
class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        g_noclipDeaths = 0;
        g_startPercent = 0;

        if (this->m_isTestMode || this->m_startPos) {
            g_startPercent = static_cast<int>(this->getCurrentPercent());
        }

        if (g_deathLabel) g_deathLabel->removeFromParentAndCleanup(true);
        g_deathLabel = CCLabelBMFont::create("Noclip Deaths: 0", "goldFont.fnt");
        g_deathLabel->setPosition({ 15.f, 230.f });
        g_deathLabel->setAnchorPoint({ 0.0f, 0.5f });
        g_deathLabel->setScale(0.5f);
        this->addChild(g_deathLabel, 10000);

        return true;
    }
};

// 3. Hook en la pantalla de victoria para añadir el texto de StartPos
class $modify(MyLevelCompleteLayer, LevelCompleteLayer) {
    void customSetup() {
        LevelCompleteLayer::customSetup();

        auto winSize = CCDirector::get()->getWinSize();
        std::string startPosText = fmt::format("StartPos: {}%", g_startPercent);
        
        auto startPosLabel = CCLabelBMFont::create(startPosText.c_str(), "goldFont.fnt");
        startPosLabel->setScale(0.6f);
        startPosLabel->setPosition({ winSize.width / 2, winSize.height / 2 - 15.f });
        
        this->addChild(startPosLabel, 100);
    }
};

// 4. Hook para la lógica del Noclip
class $modify(MyPlayerObject, PlayerObject) {
    void playerDestroyed(bool p0) {
        if (g_noclipEnabled) {
            g_noclipDeaths++;
            if (g_deathLabel) {
                g_deathLabel->setString(fmt::format("Noclip Deaths: {}", g_noclipDeaths).c_str());
            }
        }
        else {
            PlayerObject::playerDestroyed(p0);
        }
    }
};
