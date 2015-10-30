/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/
#include "player.h"
#include "worldserver.h"

CPlayer::CPlayer( CClientSocket* CLIENT )
{

    client = CLIENT;
    is_invisible=false;
    is_born=false;   //LMA: brand new player.

    pvp_id=-1;  //LMA: Pvp ID (set by qsd most of the time).

     #ifdef LMA_SPAWNM
        lastSpawnUpdate=0;
        last_monster=0;
        last_monstercid=0;
        mdeb=0;
        mend=0;
        playertime=0;
        xx=0;
        yy=0;
    #endif

    skip_qsd_zone=false;

    map_warp_zone=0;
    Warp_Zone.x=0;
    Warp_Zone.y=0;
    Warp_Zone.z=0;

    //Special AIP case.
    if (CLIENT==NULL)
    {
        is_invisible=true;
    }

    // USED ITEM
    UsedItem = new USEDITEM;
    assert(UsedItem);
    UsedItem->lastRegTime = 0;
    UsedItem->usevalue = 0;
    UsedItem->usetype = 0;
    UsedItem->userate = 0;
    UsedItem->used = 0;
    // CHARINFO
    CharInfo = new INFO;
    assert(CharInfo);
    memset( &CharInfo->charname, '\0', 17 );
	CharInfo->charid = 0;
    CharInfo->Sex = 0;
    CharInfo->Face = 0;
    CharInfo->Hair = 0;
    CharInfo->Exp = 0;
    CharInfo->Job = 0;
    CharInfo->Zulies = 0;
    CharInfo->Storage_Zulies = 0;
    CharInfo->LastGlobal = 0;
    CharInfo->StatPoints = 0;
    CharInfo->SkillPoints = 0;
    CharInfo->stamina = 0;
    CharInfo->isGM = 0; // GM Security

    // RIDE
    Ride = new RIDE;
    assert(Ride);
    Ride->Drive = false;
    Ride->Ride = false;
    Ride->charid = 0;
    // TRADE
    Trade = new TRADE;
    assert(Trade);
    for(int i=0;i<10;i++)
        Trade->trade_itemid[i] = 0;
    for(int i=0;i<11;i++)
        Trade->trade_count[i] = 0;
    Trade->trade_status = 0;
    Trade->trade_target = 0;
    //PARTY
    Party = new PARTY;
    assert(Party);
    Party->party = NULL;
    Party->IsMaster = true;
    // SHOP
    Shop = new SHOP;
    assert(Shop);
    Shop->open = false;
    memset( &Shop->name, '\0',64 );
    for(int i=0;i<30;i++)
    {
        ClearItem(Shop->BuyingList[i].item);
        Shop->BuyingList[i].slot = 0;
        Shop->BuyingList[i].count = 0;
        Shop->BuyingList[i].price = 0;
        Shop->SellingList[i].slot = 0;
        Shop->SellingList[i].count = 0;
        Shop->SellingList[i].price = 0;
    }

    Shop->Buying = 0;
    Shop->Selling = 0;
    Shop->ShopType = 0;
    Shop->mil_shop_time=0;

    //bonusxp
    timerxp=0;
    bonusxp=1;
    wait_validation=0;
    once=false;

    //LMA: Exp nullfier
    timer_no_exp=0;
    no_exp=false;

    //bonus Medal of Fortune(id.1300/type.10)_One additional drop chance for defeating a monster is added (Double Drop -> ddrop)
    timerddrop=0;
    bonusddrop=1;
    wait_validation_ddrop=0;
    once_ddrop=false;

    //bonus Medal of Excellence(id.1301/type.10)_find equipment of excellence and will result in better bonus stats than normal (statdrop)
    timerstatdrop=0;
    bonusstatdrop=1;
    wait_validation_statdrop=0;
    once_statdrop=false;

     //bonus Medal of Retrieval(id.1302/type.10)_you are able to retreive items from monsters that would normally not drop items for you. (graydrop)
    timergraydrop=0;
    bonusgraydrop=0;
    wait_validation_graydrop=0;
    once_graydrop=false;

    attack_fuel=0;
    pc_rebate=0;
    pc_craft_talent=0;
    pc_up=0;

    // SESSION
    Session = new SESSION;
    assert(Session);
	Session->userid = 0;
	memset( &Session->username, '\0', 17 );
	memset( &Session->password, '\0', 33 );
	Session->accesslevel = 0;
	Session->isLoggedIn = false;
	Session->inGame = false;
	// Inventory / storage
    for(unsigned int i=0;i<MAX_INVENTORY;i++)
        ClearItem( items[i] );

    for(unsigned int i=0;i<MAX_STORAGE;i++)
        ClearItem( storageitems[i] );

    // Clan
    Clan = new CLAN;
    assert(Clan);
    Clan->clanid = 0;
    Clan->clanrank = 0;
    Clan->grade = 0;
    Clan->logo = 0;
    Clan->back = 0;
    Clan->CP= 0;
    memset( &Clan->clanname, '\0', 17 );
    // ATTRIBUTES
    Attr = new ATTRIBUTES;
    assert(Attr);
    Attr->Str = 0;
    Attr->Dex = 0;
    Attr->Int = 0;
    Attr->Con = 0;
    Attr->Cha = 0;
    Attr->Sen = 0;
    Attr->Estr = 0;
    Attr->Edex = 0;
    Attr->Eint = 0;
    Attr->Econ = 0;
    Attr->Echa = 0;
    Attr->Esen = 0;

    CharType = TPLAYER;

    questdebug = false;
    Session->codedebug = false;
    Session->first_id=true;
    Saved = false;
    isInvisibleMode = false;
    Fairy = false;
    hits = 0;
    uw_kills=0;
    //FairyTime = 0;
    nstorageitems = 0;
    nsitemmallitems = 0;
    p_skills = 0;
    //LMA: New way
    for(int i=0;i<MAX_ALL_SKILL;i++)
    {
        cskills[i].id = 0;
        cskills[i].level = 0;
        cskills[i].thisskill=NULL;
    }

    for(int i=0;i<MAX_QUICKBAR;i++)
        quickbar[i] = 0;

    //quest.selectedNpc = NULL;
    quest.RefNPC=0; //LMA: selected NPC.
    for(int i=0;i<10;i++)
    {
        if(i<5)
        {
            quest.EpisodeVar[i]=0;
            quest.ClanVar[i]=0;
        }

        if (i<3)
        {
            quest.JobVar[i]=0;
        }

        if (i<7)
        {
            quest.PlanetVar[i]=0;
        }

        quest.UnionVar[i]=0;
        //quest.quests[i]=NULL;
    }

    for(int i=0;i<0x40;i++)
    {
        quest.flags[i]=0;
    }

    ActiveQuest = 0;
    lastRegenTime = 0;
    lastSaveTime = clock( );
    lastShowTime=0;
    firstlogin=clock();     //LMA for fairy
    lastGG = 0;
    VisiblePlayers.clear( );
    VisibleDrops.clear( );
    VisibleMonsters.clear( );
    VisibleNPCs.clear( );
}

CPlayer::~CPlayer( )
{
    if(client!=NULL)
    {
        delete client;
        client = NULL;
    }
    if(UsedItem!=NULL) delete UsedItem;
    if(CharInfo!=NULL) delete CharInfo;
    if(Ride!=NULL) delete Ride;
    if(Trade!=NULL) delete Trade;
    if(Party!=NULL) delete Party;
    if(Shop!=NULL) delete Shop;
    if(Session!=NULL) delete Session;
    if(Clan!=NULL) delete Clan;
    if(Attr!=NULL) delete Attr;

    VisiblePlayers.clear();
    VisibleDrops.clear();
    VisibleMonsters.clear();
    VisibleNPCs.clear();
}

bool CPlayer::IsMonster( )
{
    return false;
}

bool CPlayer::UpdateValues( )
{
    if( Ride->Ride && !Ride->Drive )
    {
        CPlayer* otherclient = GServer->GetClientByCID( Ride->charid, Position->Map );
        if( otherclient!=NULL )
        {
            if( otherclient->Status->Stance != 0x04 )
            {
                otherclient->Ride->Drive = false;
                otherclient->Ride->Ride = false;
                otherclient->Ride->charid = 0;
                Ride->Drive = false;
                Ride->Ride = false;
                Ride->charid = 0;
                Position->destiny = Position->current;  //ONLY ON BACK SEAT
            }
            else
            {
                Position->current = otherclient->Position->current;  //ONLY ON BACK SEAT
                Position->destiny = otherclient->Position->destiny;  //ONLY ON BACK SEAT
                Position->lastMoveTime = otherclient->Position->lastMoveTime;   //ONLY ON BACK SEAT
                return false;  // will not update the player position
            }
        }
        else
        {
            Ride->Drive = false;
            Ride->Ride = false;
            Ride->charid = 0;
            Position->destiny = Position->current;  //RIDER LOST
        }
    }

    return true;
}


//LMA: pvp code is here now, should be more clear...
long int CPlayer::ReturnPvp( CPlayer* player, CPlayer* otherclient )
{
    //otherclient is "this" and we send the packet to "player".

    //impossible case???
    if(player!=NULL&&(!(otherclient->Party->party==NULL || otherclient->Party->party != player->Party->party || otherclient->Party->party == player->Party->party)))
    {
        Log(MSG_WARNING,"Should be impossible??? returnpvp returns 0x00");
        return 0x00;
    }

    CMap* map=NULL;

    if(player!=NULL)
    {
        map = GServer->MapList.Index[player->Position->Map];
    }
    else
    {
        map = GServer->MapList.Index[otherclient->Position->Map];
    }

    //everyone should be friend
    if (map->allowpvp!=1)
    {
        //should be 2, set by QSDzone from maps at warp most of the time.
        if(pvp_id==-1)
        {
            Log(MSG_WARNING,"%s, pvp map (%i), pvp_id shouldn't be -1! Forced to 2.",CharInfo->charname,map->id);
            pvp_id=2;
        }

        //Log(MSG_WARNING,"%s, not pvp map (%i), we send pvp_id==%i",CharInfo->charname,map->id,pvp_id);
        return pvp_id;
    }

    //map is pvp
    //depends on pvp status now...
    switch (pvp_status)
    {
        case -1:
        {
            //We rely on pvp_id
            if(pvp_id==-1)
            {
                Log(MSG_WARNING,"%s, pvp map (%i), pvp_id shouldn't be -1 (2)! Forced to pvp.",CharInfo->charname,map->id);
                return (clientid + 0x100);
            }

            //Log(MSG_WARNING,"%s, pvp map (%i), we send pvp_id==%i (pvp_id)",CharInfo->charname,map->id,pvp_id);
            return pvp_id;
        }
        break;
        case 0:
        {
            //everyone is an enemy
            //Log(MSG_WARNING,"%s, pvp map (%i), we send pvp_id==%i (everyone)",CharInfo->charname,map->id,clientid + 0x100);
            return (clientid + 0x100);
        }
        break;

        case 1:
        {
            //clan is friendly
            //Log(MSG_WARNING,"%s, pvp map (%i), we send pvp_id==%i (clan)",CharInfo->charname,map->id,Clan->clanid);
            //return Clan->clanid;
            //LMA: adding an extra offset just to be sure.
            return (Clan->clanid+ 0x100);
        }

        case 2:
        {
            //party is friendly
            /*Log(MSG_WARNING,"%s, pvp map (%i), we send pvp_id==%i (party)",CharInfo->charname,map->id,otherclient->Party->party);
            return otherclient->Party->party;*/

            //LMA: for the moment, party have no ID :(
            //Log(MSG_WARNING,"PVP: we don't have an ID for party...");
            //return 0x00;

            //LMA: party have Ids now ;)
            if(Party->party!=NULL)
            {
                return (Party->party->PartyId + 0x100);
            }
            else
            {
                Log(MSG_WARNING,"Player %s should send a party pvp but isn't in any party...",CharInfo->charname);
                return (clientid + 0x100);
            }

        }

        default:
        {
            Log(MSG_WARNING,"Impossible pvp_status case (%i)",pvp_status);
        }
        break;

    }

    Log(MSG_WARNING,"ReturnPvp, we shouldn't be here");


    return 0x00;
}


// Spawn Another User on the Screen
//We send "otherclient" to "player"
bool CPlayer::SpawnToPlayer( CPlayer* player, CPlayer* otherclient )
{
    BEGINPACKET( pak, 0x793 );
	ADDWORD( pak, clientid);				// USER ID ANYONE?
    ADDFLOAT( pak, Position->current.x*100 );			// POS X
	ADDFLOAT( pak, Position->current.y*100 );			// POS Y
    ADDFLOAT( pak, Position->destiny.x*100 );			// GOING TO X
    ADDFLOAT( pak, Position->destiny.y*100 );			// GOINT TO Y
    if(Status->Stance == 0x01)
    {
        ADDWORD( pak, 0x000a );
        ADDWORD( pak, 0x0000 );
    }
    else
    if(Stats->HP <= 0)
    {
        ADDWORD( pak, 0x0003 );
        ADDWORD( pak, 0x0000 );
    }
    else
    if(Position->destiny.x != Position->current.y || Position->destiny.y != Position->current.y)
    {
        ADDWORD( pak, 0x0001 );
        ADDWORD( pak, Battle->atktarget );
    }
    else
    if(Battle->atktarget!=0)
    {
        ADDWORD( pak, 0x0002 );
        ADDWORD( pak, Battle->atktarget );
    }
    else
    {
        ADDWORD( pak, 0x0001 );
        ADDWORD( pak, 0x0000 );
    }
    switch (Status->Stance)
    {
        case WALKING:
           ADDBYTE( pak, 0x00 );
        break;
        case RUNNING:
           ADDBYTE( pak, 0x01 );
        break;
        case DRIVING:
           ADDBYTE( pak, 0x02 );
        break;
        default:
           ADDBYTE( pak, 0x0b );
    }
    ADDWORD( pak, 0x0000 );
    ADDWORD( pak, 0x0000 );

    //LMA: new simpler way.
    pvp_id=otherclient->ReturnPvp(player,otherclient);
    ADDDWORD(pak,pvp_id);

    ADDDWORD( pak, GServer->BuildBuffs( this ) );//BUFFS
    ADDBYTE( pak, CharInfo->Sex );					// GENDER
    ADDWORD( pak, Stats->Move_Speed );			// WALK SPEED MAYBE?
    ADDWORD( pak, 0 );								// ??
    ADDBYTE( pak, 0x01 );								// ??
    ADDDWORD( pak, CharInfo->Face );				// FACE TYPE
    ADDDWORD( pak, CharInfo->Hair );			// HAIR TYPE
    ADDWORD( pak, items[2].itemnum );		// CAP
    ADDWORD( pak, GServer->BuildItemRefine( items[2] ) );		// CAP REFINE
    ADDWORD( pak, items[3].itemnum );		// BODY
    ADDWORD( pak, GServer->BuildItemRefine( items[3] ) );		// BODY REFINE
    ADDWORD( pak, items[5].itemnum );		// GLOVES
    ADDWORD( pak, GServer->BuildItemRefine( items[5] )  );		// GLOVES REFINE
    ADDWORD( pak, items[6].itemnum );		// BOOTS
    ADDWORD( pak, GServer->BuildItemRefine( items[6] )  );		// BOOTS REFINE
    ADDWORD( pak, items[1].itemnum );		// FACE
    ADDWORD( pak, GServer->BuildItemRefine( items[1] )  );		// FACE REFINE
    ADDWORD( pak, items[4].itemnum );		// BACK
    ADDWORD( pak, GServer->BuildItemRefine( items[4] )  );		// BACK REFINE
    ADDWORD( pak, items[7].itemnum );		// WEAPON
    ADDWORD( pak, GServer->BuildItemRefine( items[7] )  );		// WEAPON REFINE
    ADDWORD( pak, items[8].itemnum );		// SUBWEAPON
    ADDWORD( pak, GServer->BuildItemRefine( items[8] )  );		// SUBWEAPON REFINE

    //Fix from maximz
    ADDWORD( pak, ((items[132].itemnum << 5) ));//arrows
    ADDWORD( pak, ((items[133].itemnum << 5) ));//bullets
    ADDWORD( pak, ((items[134].itemnum << 5) ));//cannons

    ADDWORD( pak, CharInfo->Job );
    ADDBYTE( pak, Stats->Level );
    ADDWORD( pak, items[135].itemnum);		// CART FRAME
    ADDWORD( pak, GServer->BuildItemRefine( items[135] ) );
    ADDWORD( pak, items[136].itemnum );	// CART ENGINE
    ADDWORD( pak, GServer->BuildItemRefine( items[136] )  );
    ADDWORD( pak, items[137].itemnum );	// CART WHEELS
    ADDWORD( pak, GServer->BuildItemRefine( items[137] )  );
    ADDWORD( pak, items[138].itemnum );	// CART WEAPON
    ADDWORD( pak, GServer->BuildItemRefine( items[138] )  );
    ADDWORD( pak, items[139].itemnum );	// CART ABILITY
    ADDWORD( pak, GServer->BuildItemRefine( items[139] )  );
	ADDWORD( pak, (Stats->HP<=0)?0x0:0xea7b );

    //LMA: new code with invisible handling
    if(Shop->open)
    {
        ADDBYTE( pak, 0x02 );
    }
    else if(otherclient->isInvisibleMode&&!player->isInvisibleMode)
    {
        Log(MSG_INFO,"Player %s should be invisible to %s",otherclient->CharInfo->charname,player->CharInfo->charname);
        ADDBYTE( pak, 0x01 );
    }
    else if(otherclient->isInvisibleMode&&player->isInvisibleMode)
    {
        //2 GMs together in hiding, just for the logs...
        Log(MSG_INFO,"Player %s and %s are both invisible but should see each other.",otherclient->CharInfo->charname,player->CharInfo->charname);
        ADDBYTE( pak, 0x00 );
    }
    else
    {
        ADDBYTE( pak, 0x00 );
    }
    //End of new code.

    ADDBYTE( pak, 0x00);
    ADDBYTE( pak, 0x00);
    if( Fairy )
    {
        ADDBYTE( pak, 0x61);
    }
    else
    {
        ADDBYTE( pak, 0x00);
    }
	ADDSTRING( pak, CharInfo->charname );
	ADDBYTE( pak, 0x00);
	if(Shop->open)
	{
	   ADDBYTE( pak, Shop->ShopType);
	   ADDBYTE( pak, 0x00);
	   ADDSTRING(pak, Shop->name);
	   ADDBYTE( pak, 0x00);       //LMA 139+
    }

    //LMA: Maxxon fix when a player is buffed.
    if (!IsDead())
    {
        //TODO: add the other buffs values?
        // correct some errors due to the map-process has not updated the players buffs yet
        // when the player is dead (it removes the buffs when the player is dead)
        // i send with the following WORDS the value the skill adds.
        if (Status->HP_up != 0xff)
        {
            ADDWORD (pak, MagicStatus[Status->HP_up].Value);    // added for test, send when P/MP UP (probably only when HP UP)
        }

        if (Status->Dash_up != 0xff)
        {
            ADDWORD (pak, MagicStatus[Status->Dash_up].Value);  // added for test, send when Dash
        }

        if (Status->Haste_up != 0xff)
        {
            ADDWORD (pak, MagicStatus[Status->Haste_up].Value); // added for test, send when Haste Attack
        }

    }
    //LMA: End of fix.

    if(Clan->clanid!=0)
    {
        ADDDWORD( pak, Clan->clanid );
        ADDWORD( pak, Clan->back);
        ADDWORD( pak, Clan->logo);
        ADDBYTE( pak, Clan->grade);
        ADDBYTE( pak, 0x00);
        ADDSTRING( pak, Clan->clanname);
        Log(MSG_INFO,"[WS] Clan info in player packet 0x793");
    }
    ADDWORD( pak, 0x0000 );
    player->client->SendPacket(&pak);
    if( Party->party!=NULL && Party->party == player->Party->party)
    {
        BEGINPACKET( pak, 0x7d5 );
        ADDDWORD   ( pak, CharInfo->charid );
        ADDWORD    ( pak, clientid );
        ADDWORD    ( pak, GetMaxHP( ) );
        ADDWORD    ( pak, Stats->HP );
        //ADDDWORD   ( pak, 0x01000000 );//Tomiz: Was not commented before
        ADDDWORD   ( pak, GServer->BuildBuffs( this ));//Tomiz: Buff Data
        //ADDDWORD   ( pak, 0x0000000f );//Tomiz: Was not commented before
        ADDDWORD   ( pak, 0x7200005d );//Tomiz
        ADDWORD    ( pak, 0x1388 );
        player->client->SendPacket( &pak );
    }
	if( Ride->Ride )
	{
        CPlayer* rideclient = GServer->GetClientByCID( Ride->charid, Position->Map );
        if(rideclient==NULL)
        {
            Ride->Ride = false;
            Ride->Drive= false;
            Ride->charid = 0;
            return true;
        }
        if( GServer->IsVisible( player, rideclient ) || player->CharInfo->charid == rideclient->CharInfo->charid )
        {
            BEGINPACKET( pak, 0x796 );
            if( Ride->Drive )
            {
                ADDWORD    ( pak, rideclient->clientid );
                ADDFLOAT   ( pak, rideclient->Position->current.x*100 );
                ADDFLOAT   ( pak, rideclient->Position->current.y*100 );
            }
            else
            {
                ADDWORD    ( pak, clientid );
                ADDFLOAT   ( pak, Position->current.x*100 );
                ADDFLOAT   ( pak, Position->current.y*100 );
            }
            ADDWORD    ( pak, 0x0000 );
            player->client->SendPacket( &pak );

            RESETPACKET( pak, 0x7dd );
            ADDBYTE    ( pak, 0x02 );
            if( rideclient->Ride->Drive )
            {
                ADDWORD    ( pak, rideclient->clientid );
                ADDWORD    ( pak, clientid );
            }
            else
            {
                ADDWORD    ( pak, clientid );
                ADDWORD    ( pak, rideclient->clientid );
            }
            player->client->SendPacket( &pak );
        }
    }
    //Little addition till the nonsense in server sync is fixed.
    //StartAction(NULL, 0, 0, true);
	return true;
}
