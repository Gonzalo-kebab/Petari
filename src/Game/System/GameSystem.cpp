#include "Game/LiveActor/Nerve.hpp"
#include "Game/NameObj/NameObjRegister.hpp"
#include "Game/System/FileRipper.hpp"
#include "Game/System/GameSystem.hpp"
#include "Game/System/GameSystemException.hpp"
#include "Game/System/GameSystemObjHolder.hpp"
#include "Game/System/GameSystemStationedArchiveLoader.hpp"
#include "Game/System/HeapMemoryWatcher.hpp"
#include "Game/Util/MathUtil.hpp"
#include "Game/Util/MemoryUtil.hpp"
#include "Game/Util/MutexHolder.hpp"
#include "Game/Util/SystemUtil.hpp"
#include "Game/SingletonHolder.hpp"
#include <nw4r/lyt/init.h>

void main(void) {
    __asm {
        li r3, 4
        oris r3, r3, 4
        mtspr 0x392, r3
        li r3, 5
        oris r3, r3, 5
        mtspr 0x393, r3
        li r3, 6
        oris r3, r3, 6
        mtspr 0x394, r3
        li r3, 7
        oris r3, r3, 7
        mtspr 0x395, r3
    };

    DVDInit();
    VIInit();
    HeapMemoryWatcher::createRootHeap();
    OSInitMutex(&MR::MutexHolder<0>::sMutex);
    OSInitMutex(&MR::MutexHolder<1>::sMutex);
    OSInitMutex(&MR::MutexHolder<2>::sMutex);
    nw4r::lyt::LytInit();
    MR::setLayoutDefaultAllocator();
    SingletonHolder<HeapMemoryWatcher>::init();
    SingletonHolder<HeapMemoryWatcher>::get()->setCurrentHeapToStationedHeap();
    FileRipper::setup(0x20000, MR::getStationedHeapNapa());
    GameSystemException::init();
    MR::initAcosTable();
    SingletonHolder<GameSystem>::init();
    SingletonHolder<GameSystem>::get()->init();

    GameSystem* pGameSystem = SingletonHolder<GameSystem>::get();

    while (true) {
        pGameSystem->frameLoop();
    }
}

namespace NrvGameSystem {
    NEW_NERVE(GameSystemInitializeAudio, GameSystem, InitializeAudio);
    NEW_NERVE(GameSystemInitializeLogoScene, GameSystem, InitializeLogoScene);
    NEW_NERVE(GameSystemLoadStationedArchive, GameSystem, LoadStationedArchive);
    NEW_NERVE(GameSystemWaitForReboot, GameSystem, WaitForReboot);
    NEW_NERVE(GameSystemNormal, GameSystem, Normal);
};

GameSystem::GameSystem() :
    NerveExecutor("GameSystem"),
    mFifoBase(NULL),
    mSequenceDirector(NULL),
    mErrorWatcher(NULL),
    mFontHolder(NULL),
    mFrameControl(NULL),
    mObjHolder(NULL),
    mSceneController(NULL),
    mStationedArchiveLoader(NULL),
    mHomeButtonLayout(NULL),
    mSystemWipeHolder(NULL),
    mHomeButtonStateNotifier(NULL),
    mIsExecuteLoadSystemArchive(false)
{
    
}

// GameSystem::init()

bool GameSystem::isExecuteLoadSystemArchive() const {
    return mIsExecuteLoadSystemArchive;
}

bool GameSystem::isDoneLoadSystemArchive() const {
    return isNerve(&NrvGameSystem::GameSystemNormal::sInstance);
}

void GameSystem::startToLoadSystemArchive() {
    mIsExecuteLoadSystemArchive = true;

    SingletonHolder<HeapMemoryWatcher>::get()->setCurrentHeapToStationedHeap();
    SingletonHolder<NameObjRegister>::get()->setCurrentHolder(mObjHolder->mObjHolder);
    setNerve(&NrvGameSystem::GameSystemLoadStationedArchive::sInstance);
}

// GameSystem::exeInitializeAudio()
// GameSystem::exeInitializeLogoScene()

void GameSystem::exeLoadStationedArchive() {
    mStationedArchiveLoader->update();
    updateSceneController();

    if (mStationedArchiveLoader->isDone()) {
        setNerve(&NrvGameSystem::GameSystemNormal::sInstance);
    }
}

void GameSystem::exeWaitForReboot() {
    
}

void GameSystem::exeNormal() {
    updateSceneController();
    mStationedArchiveLoader->update();
}

// GameSystem::initGX()
// GameSystem::initAfterStationedResourceLoaded()

void GameSystem::prepareReset() {
    mStationedArchiveLoader->prepareReset();
}

// GameSystem::isPreparedReset() const
// GameSystem::frameLoop()
// GameSystem::draw()
// GameSystem::update()
// GameSystem::updateSceneController()
// GameSystem::calcAnim()
