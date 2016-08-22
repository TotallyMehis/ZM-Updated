# Zombie Master 1.2.1 Updated
An unofficial update to Zombie Master 1.2.1 modification. This update fixes crashes and increases stability.
**This is only needed on the server. Clients can still connect to a server running this update using the normal 1.2.1 game files.**


**Changes:**

    - Fixed zombie groups. Client and server will no longer crash. zm_group_manager entity is kept through round restarts.
    - Updated lag compensation with the latest version from the wiki. Now uses the npc interp.
    - Prevents molotov crash (caused by specific player models).
    - Fixed zombie selection. It is now easier.
    - Fixed zombie spawn node recursion crash. zm_basin_b3 playable again.
    - Known issues fixed from the wiki.
    - Made drowning consistent. Player will now have ~22 seconds of air time. May need rebalancing.
    - Other minor changes.

Note: All changes made are commented with 'FIXMOD_CHANGE'. Client-project is also removed from the solution. Compiling must be done with VS2003.


**Edited files:**

    dlls/ai_concommands.cpp
    dlls/ai_basenpc.h
    dlls/ai_basenpc.cpp
    dlls/basecombatcharacter.cpp
    dlls/player.cpp
    dlls/team.cpp
    dlls/hl2_dll/npc_basezombie.cpp
    dlls/hl2_dll/grenade_molotov.cpp
    dlls/hl2mp/hl2mp_player.cpp
    dlls/player_lagcompensation.cpp
    dlls/zombielist.h
    dlls/zombielist.cpp
    dlls/zombiemaster/Zombiemaster_specific.cpp
    game_shared/func_ladder.cpp
    game_shared/gamerules.cpp
    game_shared/hl2mp/hl2mp_gamerules.cpp
    game_shared/hl2mp/weapon_zm_carry.cpp
    game_shared/hl2mp/weapon_zm_molotov.cpp
    tier1/utlbuffer.cpp
    ZombieMaster.sln