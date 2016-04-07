//=============================================================================
// Copyright (c) Zombie Master Development Team. All rights reserved.
// The use and distribution terms for this software are covered by the MIT
// License (http://opensource.org/licenses/mit-license.php) which
// can be found in the file LICENSE.TXT at the root of this distribution. By
// using this software in any fashion, you are agreeing to be bound by the
// terms of this license. You must not remove this notice, or any other, from
// this software.
//
// Note that due to the number of files included in the SDK, it is not feasible
// to include this notice in all of them. All original files or files 
// containing large modifications should contain this notice. If in doubt,
// assume the above notice applies, and refer to the included LICENSE.TXT text.
//=============================================================================
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date: 2006-11-12 11:41:44 $
//
//-----------------------------------------------------------------------------
// $Log: vgui_TeamFortressViewport.cpp,v $
// Revision 1.6  2006-11-12 11:41:44  tgb
// Fix-run of compile problems post-sourcemerge
//
// Revision 1.5  2006-07-09 18:43:01  qckbeam
// Ambush material (not active)
//
// Revision 1.4  2006-06-17 11:43:22  tgb
// GUI buttons for ZM
//
// Revision 1.3  2006-06-15 17:38:08  qckbeam
// no message
//
// Revision 1.2  2006-06-09 11:27:38  tgb
// HUD stuff, rallypoint stuff and basic nightvis
//
// Revision 1.1  2006-06-01 15:57:37  tgb
// Initial full source add, take 2
//
// Revision 1.2  2006-05-31 14:38:13  tgb
// Committing code, seeing as the one grabbed from the server had unix linebreaks or something which meant a white line between every normal line resulting in unreadability.
//
// Revision 1.6  2006/03/10 11:34:48  theGreenBunny
// crash fixes
//
// Revision 1.5  2006/03/03 10:05:09  theGreenBunny
// no message
//
// Revision 1.4  2006/03/01 04:40:52  theGreenBunny
// no message
//
// Revision 1.3  2006/01/31 15:20:15  qckbeam
// no message
//
// Revision 1.2  2006/01/19 00:17:20  qckbeam
// no message
//
// Revision 1.1.1.1  2005/10/11 14:57:51  theGreenBunny
// The latest version of the code for zombie master
//
//
// $NoKeywords: $
//=============================================================================//

#pragma warning( disable : 4800  )  // disable forcing int to bool performance warning

#include "cbase.h"
#include <cdll_client_int.h>
#include <cdll_util.h>
#include <globalvars_base.h>

// VGUI panel includes
#include <vgui_controls/Panel.h>
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui/ipanel.h>

#include <keydefs.h> // K_ENTER, ... define
#include <igameresources.h>

// sub dialogs
#include "clientscoreboarddialog.h"
#include "spectatorgui.h"
#include "teammenu.h"
#include "vguitextwindow.h"
#include "IGameUIFuncs.h"
#include "mapoverview.h"
#include "hud.h"
#include "NavProgress.h"

// our definition
#include "baseviewport.h"
#include <filesystem.h>
#include <convar.h>
#include <sdk/vgui/vgui_viewport.h> //LAWYER:  ZombieMaster interface
#include <game_controls/buildmenu.h> //LAWYER:  ZombieMaster interface
#include <game_controls/manipulatemenu.h> //LAWYER:  ZombieMaster interface
#include <game_controls/startmenu.h> //LAWYER:  ZombieMaster interface
#include <menu_ambush.h>
#include <game_controls/radiopanel.h>
#include <zm_controlpanel.h> //qck: ZM Control Panel

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IViewPort *gViewPortInterface = NULL;

vgui::Panel *g_lastPanel = NULL; // used for mouseover buttons, keeps track of the last active panel
using namespace vgui;

ConVar hud_autoreloadscript("hud_autoreloadscript", "0", FCVAR_NONE, "Automatically reloads the animation script each time one is ran");

static ConVar cl_leveloverviewmarker( "cl_leveloverviewmarker", "0", FCVAR_CHEAT );

CON_COMMAND( showpanel, "Shows a viewport panel <name>" )
{
	if ( !gViewPortInterface )
		return;
	
	if ( engine->Cmd_Argc() != 2 )
		return;
		
	 gViewPortInterface->ShowPanel( engine->Cmd_Argv( 1 ), true );
}

CON_COMMAND( hidepanel, "Hides a viewport panel <name>" )
{
	if ( !gViewPortInterface )
		return;
	
	if ( engine->Cmd_Argc() != 2 )
		return;
		
	 gViewPortInterface->ShowPanel( engine->Cmd_Argv( 1 ), false );
}

CON_COMMAND( zombiepanel_open, "Opens the Zombie Master interface" )
{
 if ( !gViewPortInterface )
  return;
 	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer->GetTeamNumber() == 3)
	{
		gViewPortInterface->ShowPanel( "vgui_viewport", true ); //Lawyer: We need checks to see if it's the Zombie Master
	}
	else
	{
		Msg("You're not a Zombie Master, therefore can't do that.");
	}
//	Warning("Opening viewport");

}

CON_COMMAND( zombiepanel_close, "Closes the Zombie Master interface" )
{
 if ( !gViewPortInterface )
  return;
 gViewPortInterface->ShowPanel( "vgui_viewport", false ); //Lawyer: We need checks to see if it's the Zombie Master
//	Warning("Opening viewport");

}

/*
CON_COMMAND( buildmenu, "Opens a menu for building units" ) //LAWYER:  Open a buildmenu!
{
 if ( !gViewPortInterface )
  return;
 gViewPortInterface->ShowPanel( "build", true );
}


CON_COMMAND( buildmenu_alt, "Opens options menu for a zombie spawn") //qck: Open up our spawn options menu.
{
//TGB: unused
	if( !gViewPortInterface )
		return;
	gViewPortInterface->ShowPanel( "alt_build", true );

}
*/

//TGB: no longer needed now that we have hud buttons
//CON_COMMAND( zm_controlpanel, "Opens the zombie master control panel") //qck: Open up the window from which the zm derives all power
//{
//	if( !gViewPortInterface )
//		return;
//	gViewPortInterface->ShowPanel( "zm_controlpanel", true);
//}

CON_COMMAND( manipulatemenu, "Opens a menu for manipulation" ) //LAWYER:  Open a buildmenu!
{
 if ( !gViewPortInterface )
  return;
 gViewPortInterface->ShowPanel( "manipulate", true );
}

CON_COMMAND( startmenu, "Opens the starting menu" ) //LAWYER:  Open a buildmenu!
{
 if ( !gViewPortInterface )
  return;
 gViewPortInterface->ShowPanel( "startmenu", true );
}
/* global helper functions

bool Helper_LoadFile( IBaseFileSystem *pFileSystem, const char *pFilename, CUtlVector<char> &buf )
{
	FileHandle_t hFile = pFileSystem->Open( pFilename, "rt" );
	if ( hFile == FILESYSTEM_INVALID_HANDLE )
	{
		Warning( "Helper_LoadFile: missing %s\n", pFilename );
		return false;
	}

	unsigned long len = pFileSystem->Size( hFile );
	buf.SetSize( len );
	pFileSystem->Read( buf.Base(), buf.Count(), hFile );
	pFileSystem->Close( hFile );

	return true;
} */


//================================================================
CBaseViewport::CBaseViewport() : vgui::EditablePanel( NULL, "CBaseViewport")
{
	gViewPortInterface = this;
	m_bInitialized = false;

	//m_PendingDialogs.Purge();
	m_GameuiFuncs = NULL;
	m_GameEventManager = NULL;
	SetKeyBoardInputEnabled( false );
	SetMouseInputEnabled( false );

	m_pBackGround = NULL;
	m_bHasParent = false;
	m_pActivePanel = NULL;
	m_pLastActivePanel = NULL;

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", "ClientScheme");
	SetScheme(scheme);
	SetProportional( true );

	m_pAnimController = new vgui::AnimationController(this);
	// create our animation controller
	m_pAnimController->SetScheme(scheme);
	m_pAnimController->SetProportional(true);
	if (!m_pAnimController->SetScriptFile(this->GetVPanel(), "scripts/HudAnimations.txt"))
	{
		Assert(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates hud to handle the new screen size
//-----------------------------------------------------------------------------
void CBaseViewport::OnScreenSizeChanged(int iOldWide, int iOldTall)
{
	BaseClass::OnScreenSizeChanged(iOldWide, iOldTall);

	// reload the script file, so the screen positions in it are correct for the new resolution
	ReloadScheme( NULL );

	// recreate all the default panels
	RemoveAllPanels();
	m_pBackGround = new CBackGroundPanel( NULL );
	m_pBackGround->SetZPos( -20 ); // send it to the back 
	m_pBackGround->SetVisible( false );
	CreateDefaultPanels();
}

void CBaseViewport::CreateDefaultPanels( void )
{
	DevMsg("Viewport creating default panels\n");
	AddNewPanel( CreatePanelByName( PANEL_SCOREBOARD ) );
	AddNewPanel( CreatePanelByName( PANEL_INFO ) );
	AddNewPanel( CreatePanelByName( PANEL_SPECGUI ) );
	AddNewPanel( CreatePanelByName( PANEL_SPECMENU ) );
	AddNewPanel( CreatePanelByName( PANEL_NAV_PROGRESS ) );
	AddNewPanel( CreatePanelByName( PANEL_VIEWPORT ) ); //LAWYER:  The Viewport Zombie
	AddNewPanel( CreatePanelByName( PANEL_BUILD ) ); //LAWYER:  The build menu
	AddNewPanel( CreatePanelByName( PANEL_MANIPULATE ) ); //LAWYER:  The manipulate menu
	AddNewPanel( CreatePanelByName( PANEL_AMBUSH ) );
	AddNewPanel( CreatePanelByName( PANEL_START ) ); //LAWYER:  The start
	//AddNewPanel( CreatePanelByName( PANEL_ALTBUILD ) ); //qck: The spawn options menu
	//AddNewPanel( CreatePanelByName( PANEL_ZMCP ) ); //qck: The ZM control panel
	//AddNewPanel( CreatePanelByName( PANEL_RADIO ) );
	//AddNewPanel( CreatePanelByName( PANEL_OVERVIEW ) );	//TGB: testing
	// AddNewPanel( CreatePanelByName( PANEL_TEAM ) );
	// AddNewPanel( CreatePanelByName( PANEL_CLASS ) );
	// AddNewPanel( CreatePanelByName( PANEL_BUY ) );
}

void CBaseViewport::UpdateAllPanels( void )
{
	int count = m_Panels.Count();

	for (int i=0; i< count; i++ )
	{
		IViewPortPanel *p = m_Panels[i];

		if ( p->IsVisible() )
		{
			p->Update();
		}
	}
}

IViewPortPanel* CBaseViewport::CreatePanelByName(const char *szPanelName)
{
	IViewPortPanel* newpanel = NULL;
	DevMsg("Creating panel by name: %s\n", szPanelName); 

	if ( Q_strcmp(PANEL_SCOREBOARD, szPanelName) == 0 )
	{
		newpanel = new CClientScoreBoardDialog( this );
	}
	else if ( Q_strcmp(PANEL_INFO, szPanelName) == 0 )
	{
		newpanel = new CTextWindow( this );
	}
	//TGB: screwed up post-merge, and don't think we use it anyway
	//else if ( Q_strcmp(PANEL_OVERVIEW, szPanelName) == 0 )
	//{
	//	newpanel = new CMapOverview( this );
	//}
	else if ( Q_strcmp(PANEL_TEAM, szPanelName) == 0 )
	{
		newpanel = new CTeamMenu( this );
	}
	else if ( Q_strcmp(PANEL_SPECMENU, szPanelName) == 0 )
	{
		newpanel = new CSpectatorMenu( this );
	}
	else if ( Q_strcmp(PANEL_SPECGUI, szPanelName) == 0 )
	{
		newpanel = new CSpectatorGUI( this );
	}
	else if ( Q_strcmp(PANEL_NAV_PROGRESS, szPanelName) == 0 )
	{
		newpanel = new CNavProgress( this );
	}
	else if ( Q_strcmp(PANEL_VIEWPORT, szPanelName) == 0 )
	{
		newpanel = new CBaseZombieMasterViewPort( this ); //LAWYER:  More Zombie control
	}
	else if ( Q_strcmp(PANEL_BUILD, szPanelName) == 0 )
	{

		newpanel = new CBuildMenu( this ); //LAWYER: Build zombies!
	}
	/*TGB: unused
	else if ( Q_strcmp(PANEL_ALTBUILD, szPanelName) == 0)
	{
		newpanel = new CAltBuildMenu( this ); //qck: Bring up the zombie spawn options menu;
	}*/
	else if ( Q_strcmp(PANEL_ZMCP, szPanelName) == 0)
	{
		newpanel = new CZM_ControlPanel( this ); //qck: Bring up the zombie master control panel
	}
	else if ( Q_strcmp(PANEL_MANIPULATE, szPanelName) == 0 )
	{
		newpanel = new CManipulateMenu( this ); //LAWYER: Control manipulates!
	}
	else if ( Q_strcmp(PANEL_START, szPanelName) == 0 )
	{
		newpanel = new CStartMenu( this );
	}
	else if (Q_strcmp(PANEL_AMBUSH, szPanelName) == 0 )
	{
		newpanel = new CAmbushMenu( this );
	}
	else if ( Q_strcmp(PANEL_RADIO, szPanelName) == 0 )
	{
		newpanel = new CRadioMenu( this ); //TGB: radio menu
	}

	return newpanel; 
}


bool CBaseViewport::AddNewPanel( IViewPortPanel* pPanel )
{
	DevMsg("CBviewp adding new panel\n");
	if ( !pPanel )
	{
		DevMsg("CBaseViewport::AddNewPanel: NULL panel.\n" );
		return false;
	}

	// we created a new panel, initialize it
	if ( FindPanelByName( pPanel->GetName() ) != NULL )
	{
		DevMsg("CBaseViewport::AddNewPanel: panel with name '%s' already exists.\n", pPanel->GetName() );
		return false;
	}

	m_Panels.AddToTail( pPanel );
	pPanel->SetParent( GetVPanel() );
	
	return true;
}

IViewPortPanel* CBaseViewport::FindPanelByName(const char *szPanelName)
{
	int count = m_Panels.Count();

	for (int i=0; i< count; i++ )
	{
		if ( Q_strcmp(m_Panels[i]->GetName(), szPanelName) == 0 )
			return m_Panels[i];
	}

	return NULL;
}

void CBaseViewport::ShowPanel( const char *pName, bool state )
{
	if ( Q_strcmp( pName, PANEL_ALL ) == 0 )
	{
		for (int i=0; i< m_Panels.Count(); i++ )
		{
			ShowPanel( m_Panels[i], state );
		}

		return;
	}

	IViewPortPanel * panel = NULL;

	if ( Q_strcmp( pName, PANEL_ACTIVE ) == 0 )
	{
		panel = m_pActivePanel;
	}
	else
	{
		panel = FindPanelByName( pName );
	}


	if ( !panel	)
		return;

	ShowPanel( panel, state );
}

void CBaseViewport::ShowPanel( IViewPortPanel* pPanel, bool state )
{
	if ( state )
	{
		// if this is an 'active' panel, deactivate old active panel
		if ( pPanel->HasInputElements() )
		{
			// don't show input panels during normal demo playback
			if ( engine->IsPlayingDemo() && !engine->IsHLTV() )
				return;

			if ( (m_pActivePanel != NULL) && (m_pActivePanel != pPanel) )
			{
				// store a pointer to the currently active panel
				// so we can restore it later
				m_pLastActivePanel = m_pActivePanel;
				m_pActivePanel->ShowPanel( false );
			}
		
			m_pActivePanel = pPanel;
		}
	}
	else
	{
		//TGB: no last panel crapping around if we're dealing with the viewport
		if ( Q_strcmp( pPanel->GetName(), PANEL_VIEWPORT ) == 0 )
			m_pLastActivePanel = NULL;

		// if this is our current active panel
		// update m_pActivePanel pointer
		if ( m_pActivePanel == pPanel )
		{
			m_pActivePanel = NULL;
		}

		// restore the previous active panel if it exists
		if( m_pLastActivePanel )
		{
			m_pActivePanel = m_pLastActivePanel;
			m_pLastActivePanel = NULL;

			m_pActivePanel->ShowPanel( true );
		}
	}

	// just show/hide panel
	pPanel->ShowPanel( state );

	UpdateAllPanels(); // let other panels rearrange
}

IViewPortPanel* CBaseViewport::GetActivePanel( void )
{
	return m_pActivePanel;
}

void CBaseViewport::RemoveAllPanels( void)
{
	DevMsg("CBaseViewport::RemoveAllPanels, panel list has %i elements\n", m_Panels.Count());
	

	for ( int i=0; i < m_Panels.Count(); i++ )
	{
		vgui::VPANEL vPanel = m_Panels[i]->GetVPanel();
		DevMsg("CBaseViewport removing %s with vpanel %i\n", m_Panels[i]->GetName(), vPanel  );

		vgui::ipanel()->DeletePanel( vPanel );
		
	}

	if ( m_pBackGround )
	{
		m_pBackGround->MarkForDeletion();
		m_pBackGround = NULL;
	}

	m_Panels.Purge();
	m_pActivePanel = NULL;
	m_pLastActivePanel = NULL;
}

CBaseViewport::~CBaseViewport()
{
	m_bInitialized = false;

	RemoveAllPanels();
}


//-----------------------------------------------------------------------------
// Purpose: called when the VGUI subsystem starts up
//			Creates the sub panels and initialises them
//-----------------------------------------------------------------------------
void CBaseViewport::Start( IGameUIFuncs *pGameUIFuncs, IGameEventManager2 * pGameEventManager )
{
	m_GameuiFuncs = pGameUIFuncs;
	m_GameEventManager = pGameEventManager;

	m_pBackGround = new CBackGroundPanel( NULL );
	m_pBackGround->SetZPos( -20 ); // send it to the back 
	m_pBackGround->SetVisible( false );

	CreateDefaultPanels();

	m_GameEventManager->AddListener( this, "game_newmap", false );
	
	m_bInitialized = true;
}

//TGB FIXME: why is this commented? lack of this related to player info not updating when observing someone?
/*

//-----------------------------------------------------------------------------
// Purpose: Updates the spectator panel with new player info
/*-----------------------------------------------------------------------------
void CBaseViewport::UpdateSpectatorPanel()
{
	char bottomText[128];
	int player = -1;
	const char *name;
	Q_snprintf(bottomText,sizeof( bottomText ), "#Spec_Mode%d", m_pClientDllInterface->SpectatorMode() );

	m_pClientDllInterface->CheckSettings();
	// check if we're locked onto a target, show the player's name
	if ( (m_pClientDllInterface->SpectatorTarget() > 0) && (m_pClientDllInterface->SpectatorTarget() <= m_pClientDllInterface->GetMaxPlayers()) && (m_pClientDllInterface->SpectatorMode() != OBS_ROAMING) )
	{
		player = m_pClientDllInterface->SpectatorTarget();
	}

		// special case in free map and inset off, don't show names
	if ( ((m_pClientDllInterface->SpectatorMode() == OBS_MAP_FREE) && !m_pClientDllInterface->PipInsetOff()) || player == -1 )
		name = NULL;
	else
		name = m_pClientDllInterface->GetPlayerInfo(player).name;

	// create player & health string
	if ( player && name )
	{
		Q_strncpy( bottomText, name, sizeof( bottomText ) );
	}
	char szMapName[64];
	Q_FileBase( const_cast<char *>(m_pClientDllInterface->GetLevelName()), szMapName );

	m_pSpectatorGUI->Update(bottomText, player, m_pClientDllInterface->SpectatorMode(), m_pClientDllInterface->IsSpectateOnly(), m_pClientDllInterface->SpectatorNumber(), szMapName );
	m_pSpectatorGUI->UpdateSpectatorPlayerList();
}  */

// Return TRUE if the HUD's allowed to print text messages
bool CBaseViewport::AllowedToPrintText( void )
{

	/* int iId = GetCurrentMenuID();
	if ( iId == MENU_TEAM || iId == MENU_CLASS || iId == MENU_INTRO || iId == MENU_CLASSHELP )
		return false; */
	// TODO ask every aktive elemet if it allows to draw text while visible

	return ( m_pActivePanel == NULL);
} 

void CBaseViewport::OnThink()
{
	// Clear our active panel pointer if the panel has made
	// itself invisible. Need this so we don't bring up dead panels
	// if they are stored as the last active panel
	if( m_pActivePanel && !m_pActivePanel->IsVisible() )
	{
		if( m_pLastActivePanel )
		{
			m_pActivePanel = m_pLastActivePanel;
			ShowPanel( m_pActivePanel, true );
		}
		else
			m_pActivePanel = NULL;
	}
	
	m_pAnimController->UpdateAnimations( gpGlobals->curtime );

	// check the auto-reload cvar
	m_pAnimController->SetAutoReloadScript(hud_autoreloadscript.GetBool());

	int count = m_Panels.Count();

	for (int i=0; i< count; i++ )
	{
		IViewPortPanel *panel = m_Panels[i];
		if ( panel->NeedsUpdate() && panel->IsVisible() )
		{
			panel->Update();
		}
	}

	BaseClass::OnThink();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the parent for each panel to use
//-----------------------------------------------------------------------------
void CBaseViewport::SetParent(vgui::VPANEL parent)
{
	EditablePanel::SetParent( parent );

	m_pBackGround->SetParent( (vgui::VPANEL)parent );

	for (int i=0; i< m_Panels.Count(); i++ )
	{
		m_Panels[i]->SetParent( parent );
	}

	m_bHasParent = (parent != 0);
}

//-----------------------------------------------------------------------------
// Purpose: called when the engine shows the base client VGUI panel (i.e when entering a new level or exiting GameUI )
//-----------------------------------------------------------------------------
void CBaseViewport::ActivateClientUI() 
{
}

//-----------------------------------------------------------------------------
// Purpose: called when the engine hides the base client VGUI panel (i.e when the GameUI is comming up ) 
//-----------------------------------------------------------------------------
void CBaseViewport::HideClientUI()
{
}

//-----------------------------------------------------------------------------
// Purpose: passes death msgs to the scoreboard to display specially
//-----------------------------------------------------------------------------
void CBaseViewport::FireGameEvent( IGameEvent * event)
{
	const char * type = event->GetName();

	if ( Q_strcmp(type, "game_newmap") == 0 )
	{
		// hide all panels when reconnecting 
		ShowPanel( PANEL_ALL, false );

		if ( engine->IsHLTV() )
		{
			ShowPanel( PANEL_SPECGUI, true );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseViewport::ReloadScheme(const char *fromFile)
{
	// See if scheme should change
	
	if ( fromFile != NULL )
	{
		// "resource/ClientScheme.res"
		vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( fromFile, "HudScheme" );

		SetScheme(scheme);
		SetProportional( true );
		m_pAnimController->SetScheme(scheme);
	}

	// Force a reload
	if ( !m_pAnimController->SetScriptFile(this->GetVPanel(), "scripts/HudAnimations.txt", true) )
	{
		Assert( 0 );
	}

	SetProportional( true );
	
	// reload the .res file from disk
	LoadControlSettings("scripts/HudLayout.res");

	gHUD.RefreshHudTextures();

	InvalidateLayout( true, true );

	// reset the hud
	gHUD.ResetHUD();
}

int CBaseViewport::GetDeathMessageStartHeight( void )
{
	return YRES(2);
}

void CBaseViewport::Paint()
{
	if ( cl_leveloverviewmarker.GetInt() > 0 )
	{
		int size = cl_leveloverviewmarker.GetInt();
		// draw a 1024x1024 pixel box
		vgui::surface()->DrawSetColor( 255, 0, 0, 255 );
		vgui::surface()->DrawLine( size, 0, size, size );
		vgui::surface()->DrawLine( 0, size, size, size );
	}
}
