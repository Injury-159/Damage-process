#if defined damage_process
	#endinput
#endif
#define damage_process

#include <YSI_Coding\y_hooks>


#define MAX_WEAPONS 									(55)

#define WEAPON_UNARMED 									(0)
#define WEAPON_VEHICLE_M4 								(19)
#define WEAPON_VEHICLE_MINIGUN 							(20)
#define WEAPON_VEHICLE_ROCKETLAUNCHER 					(21)
#define WEAPON_PISTOLWHIP 								(48)
#define WEAPON_HELIBLADES 								(50)
#define WEAPON_EXPLOSION 								(51)
#define WEAPON_CARPARK 									(52)
#define WEAPON_UNKNOWN 									(55)

#define DP_IsMelee(%0) \
	((WEAPON_UNARMED <= (%0) <= WEAPON_KATANA) || (WEAPON_DILDO <= (%0) <= WEAPON_CANE) || (%0) == WEAPON_PISTOLWHIP)	


/* ————————————————————————————————————————————————————————————————

STARTING

———————————————————————————————————————————————————————————————— */	


#if defined OnPlayerDamage
	forward OnPlayerDamage(playerid, issuerid, &Float:amount, weapon, bodypart);
#endif


forward Float:GetPlayerWeaponDamage(playerid, weaponid); 	



/* ————————————————————————————————————————————————————————————————

DATA

———————————————————————————————————————————————————————————————— */	


enum E_DEATH_DATA 
{
	bool:dpValid,
	dpKillerid,
	dpReason
};

static 
	Float:DP_WeaponDamage[MAX_PLAYERS][MAX_WEAPONS],
	DP_DeathData[MAX_PLAYERS][E_DEATH_DATA],
	DP_LastExplosive[MAX_PLAYERS],
	DP_TeamID[MAX_PLAYERS],
	DP_KnifeTimer[MAX_PLAYERS],

	Float:DP_PlayerHealth[MAX_PLAYERS];


/* ————————————————————————————————————————————————————————————————

FUNCTIONS

———————————————————————————————————————————————————————————————— */	


// by Slice
static DP_ProcessDamage(playerid, issuerid, &Float:amount, &weaponid, &bodypart) 
{ 
	switch (amount) 
	{
		case 3.63000011444091796875, 5.940000057220458984375, 5.610000133514404296875: 
	    {
			amount = 2.6400001049041748046875;
		}

		case 3.30000019073486328125: 
		{
			if (weaponid != WEAPON_SHOTGUN && weaponid != WEAPON_SAWEDOFF)
				amount = 2.6400001049041748046875;
		}

		case 4.950000286102294921875: 
		{
			if (DP_IsMelee(weaponid))
				amount = 2.6400001049041748046875;
		}

		case 6.270000457763671875, 6.93000030517578125, 7.2600002288818359375, 7.9200000762939453125, 8.5799999237060546875, 9.24000072479248046875, 11.88000011444091796875, 11.22000026702880859375: 
	    {
			amount = 2.6400001049041748046875;
		}

		case 9.90000057220458984375: 
		{
			switch (weaponid) 
			{
				case WEAPON_VEHICLE, WEAPON_VEHICLE_M4, WEAPON_AK47, WEAPON_M4, WEAPON_SHOTGUN, WEAPON_SAWEDOFF, WEAPON_SHOTGSPA: 
				{}

				default: 
				{
					amount = 6.6000003814697265625;
				}
			}
		}
	}

	if (weaponid == WEAPON_HELIBLADES && _:amount != _:330.0)
		weaponid = WEAPON_CARPARK;


	switch (weaponid) 
	{
		case WEAPON_FLAMETHROWER, WEAPON_SPRAYCAN, WEAPON_FIREEXTINGUISHER: 
		{
			if (amount > 1.0) 
				amount = 1.0;

			amount *= DP_WeaponDamage[playerid][weaponid];

			new 
				Float:x, 
				Float:y, 
				Float:z;

			GetPlayerPos(issuerid, x, y, z);
			if (GetPlayerDistanceFromPoint(playerid, x, y, z) > 15.0)
				return 0;

			return 1;
		}
	}

	if(weaponid == WEAPON_PARACHUTE) 
		weaponid = WEAPON_UNARMED;


	if(weaponid == WEAPON_EXPLOSION)
	{	
		amount /= 82.5;

		weaponid = DP_LastExplosive[issuerid];
	}

	switch (weaponid) 
	{
		case WEAPON_COLT45 .. WEAPON_SNIPER, WEAPON_MINIGUN, WEAPON_SPRAYCAN, WEAPON_FIREEXTINGUISHER: 
		{
			if (_:amount == _:2.6400001049041748046875) 
			{
				bodypart = weaponid;
				weaponid = WEAPON_PISTOLWHIP;
			}
		}
	}

	if (!DP_IsMelee(weaponid)) 
	{
		switch (amount) 
		{
			case 1.32000005245208740234375, 1.650000095367431640625, 1.980000019073486328125, 2.3100001811981201171875, 2.6400001049041748046875, 2.9700000286102294921875, 3.96000003814697265625, 4.28999996185302734375, 4.62000036239624023437, 5.280000209808349609375: 
			{
				weaponid = WEAPON_UNARMED;
			}

			case 6.6000003814697265625: 
			{
				switch (weaponid) 
				{
					case WEAPON_UZI, WEAPON_TEC9, WEAPON_CHAINSAW, WEAPON_SHOTGUN, WEAPON_SAWEDOFF: 
					{}

					default: 
					{
						weaponid = WEAPON_UNARMED;
					}
				}
			}

			case 54.12000274658203125: 
			{
				weaponid = WEAPON_UNARMED;
				amount = 1.32000005245208740234375;
			}
		}
	}

	if (DP_IsMelee(weaponid)) 
	{
		new 
			Float:x, 
			Float:y, 
			Float:z;

		GetPlayerPos(issuerid, x, y, z);
		if (GetPlayerDistanceFromPoint(playerid, x, y, z) > 15.0)
			return 0;
	}

	new  Float:bullets = 0.0;

	switch (weaponid) 
	{
		case WEAPON_SHOTGSPA: 
		{
			bullets = amount / 4.950000286102294921875;

			if (8.0 - bullets < -0.05)
				return 0;
		}

		case WEAPON_SHOTGUN, WEAPON_SAWEDOFF: 
		{
			bullets = amount / 3.30000019073486328125;

			if (15.0 - bullets < -0.05)
				return 0;
		}
	}

	if (_:bullets) 
	{
		new Float:f = floatfract(bullets);

		if (f > 0.01 && f < 0.99)
			return 0;

		amount /= bullets;
	}


	if (weaponid == WEAPON_CHAINSAW) 
	{
		switch (amount) 
		{
			case 6.6000003814697265625, 13.5300006866455078125, 16.1700000762939453125, 26.40000152587890625, 27.060001373291015625: 
			{}

			default: 
			{
				return 0;
			}
		}
	}

	if (weaponid == WEAPON_DEAGLE) 
	{
		switch (amount) 
		{
			case 46.200000762939453125, 23.1000003814697265625: 
			{}

			default: 
			{
				return 0;
			}
		}
	}

	new Float:def_amount = 0.0;

	switch (weaponid) 
	{
		case WEAPON_COLT45, WEAPON_MP5: 					def_amount = 8.25;
		case WEAPON_SILENCED: 								def_amount = 13.200000762939453125;
		case WEAPON_UZI, WEAPON_TEC9: 						def_amount = 6.6000003814697265625;
		case WEAPON_AK47, WEAPON_M4, WEAPON_VEHICLE_M4: 	def_amount = 9.90000057220458984375;
		case WEAPON_RIFLE: 									def_amount = 24.7500019073486328125;
		case WEAPON_SNIPER: 								def_amount = 41.25;
		case WEAPON_MINIGUN, WEAPON_VEHICLE_MINIGUN: 		def_amount = 46.200000762939453125;
		case WEAPON_VEHICLE: 								def_amount = 9.90000057220458984375;
	}

	if (_:def_amount && _:def_amount != _:amount)
		return 0;


	switch (weaponid) 
	{
		case WEAPON_UNARMED..WEAPON_KATANA, WEAPON_DILDO..WEAPON_CANE, WEAPON_PISTOLWHIP: 
		{
			amount = DP_WeaponDamage[issuerid][weaponid];
		}


		case WEAPON_CHAINSAW: 
		{
			amount *= DP_WeaponDamage[issuerid][weaponid];
		}


		case WEAPON_VEHICLE_M4, WEAPON_VEHICLE_MINIGUN: 
		{
			amount = DP_WeaponDamage[issuerid][weaponid];
		}


		case WEAPON_SHOTGSPA, WEAPON_SHOTGUN, WEAPON_SAWEDOFF: 
		{
			amount = (bullets * DP_WeaponDamage[issuerid][weaponid]);
		}


		case WEAPON_COLT45..WEAPON_DEAGLE, WEAPON_UZI..WEAPON_SNIPER, WEAPON_MINIGUN: 
		{
			amount = DP_WeaponDamage[issuerid][weaponid];
		}
	}
	return 1;
}


static DP_InflictDamage(playerid, Float:amount, bool:ignore_armour = false) 
{
	if (!DP_IsPlayerSpawned(playerid))
		return;

	new 
		Float:armour, 
		Float:health = DP_PlayerHealth[playerid];


	GetPlayerArmour(playerid, armour);

	if (ignore_armour == false) 
	{
		armour -= amount;
		if (armour < 0.0) 
			health += armour;
	}
	else 
	{
	    health -= amount;
	}

	if (armour < 0.0) 
		armour = 0.0;

	DP_PlayerHealth[playerid] = (health < 0.0 ? 0.0 : health);

	if(DP_PlayerHealth[playerid] <= 0.0) 
		SetPlayerHealth(playerid, DP_PlayerHealth[playerid]);

	SetPlayerArmour(playerid, armour);
}


hook OnPlayerConnect(playerid) 
{
	DP_WeaponDamage[playerid][0] = 1.0; 	// 0 - Fist
	DP_WeaponDamage[playerid][1] = 1.0; 	// 1 - Brass knuckles
	DP_WeaponDamage[playerid][2] = 1.0; 	// 2 - Golf club
	DP_WeaponDamage[playerid][3] = 1.0; 	// 3 - Nitestick
	DP_WeaponDamage[playerid][4] = 1.0; 	// 4 - Knife
	DP_WeaponDamage[playerid][5] = 1.0; 	// 5 - Bat
	DP_WeaponDamage[playerid][6] = 1.0; 	// 6 - Shovel
	DP_WeaponDamage[playerid][7] = 1.0; 	// 7 - Pool cue
	DP_WeaponDamage[playerid][8] = 1.0; 	// 8 - Katana
	DP_WeaponDamage[playerid][9] = 1.0; 	// 9 - Chainsaw
	DP_WeaponDamage[playerid][10] = 1.0; 	// 10 - Dildo
	DP_WeaponDamage[playerid][11] = 1.0; 	// 11 - Dildo 2
	DP_WeaponDamage[playerid][12] = 1.0; 	// 12 - Vibrator
	DP_WeaponDamage[playerid][13] = 1.0; 	// 13 - Vibrator 2
	DP_WeaponDamage[playerid][14] = 1.0; 	// 14 - Flowers
	DP_WeaponDamage[playerid][15] = 1.0; 	// 15 - Cane
	DP_WeaponDamage[playerid][16] = 82.5; 	// 16 - Grenade
	DP_WeaponDamage[playerid][18] = 1.0; 	// 18 - Molotov
	DP_WeaponDamage[playerid][19] = 9.9; 	// 19 - Vehicle M4 (custom)
	DP_WeaponDamage[playerid][20] = 46.2; 	// 20 - Vehicle minigun (custom)
	DP_WeaponDamage[playerid][21] = 82.5; 	// 21 - Vehicle rocket (custom)
	DP_WeaponDamage[playerid][22] = 8.25; 	// 22 - Colt 45
	DP_WeaponDamage[playerid][23] = 13.2; 	// 23 - Silenced
	DP_WeaponDamage[playerid][24] = 46.2; 	// 24 - Deagle
	DP_WeaponDamage[playerid][25] = 3.3; 	// 25 - Shotgun
	DP_WeaponDamage[playerid][26] = 3.3; 	// 26 - Sawed-off
	DP_WeaponDamage[playerid][27] = 4.95; 	// 27 - Spas
	DP_WeaponDamage[playerid][28] = 6.6; 	// 28 - UZI
	DP_WeaponDamage[playerid][29] = 8.25; 	// 29 - MP5
	DP_WeaponDamage[playerid][30] = 9.9; 	// 30 - AK47
	DP_WeaponDamage[playerid][31] = 9.9; 	// 31 - M4
	DP_WeaponDamage[playerid][32] = 6.6; 	// 32 - Tec9
	DP_WeaponDamage[playerid][33] = 24.75; 	// 33 - Cuntgun
	DP_WeaponDamage[playerid][34] = 41.25;  // 34 - Sniper
	DP_WeaponDamage[playerid][35] = 82.5; 	// 35 - Rocket launcher
	DP_WeaponDamage[playerid][36] = 82.5; 	// 36 - Heatseeker
	DP_WeaponDamage[playerid][37] = 1.0; 	// 37 - Flamethrower
	DP_WeaponDamage[playerid][38] = 46.2; 	// 38 - Minigun
	DP_WeaponDamage[playerid][39] = 82.5; 	// 39 - Satchel
	DP_WeaponDamage[playerid][41] = 0.33; 	// 41 - Spraycan
	DP_WeaponDamage[playerid][42] = 0.33; 	// 42 - Fire extinguisher
	DP_WeaponDamage[playerid][48] = 2.64; 	// 48 - Pistol whip (custom)
	DP_WeaponDamage[playerid][49] = 9.9; 	// 49 - Vehicle
	DP_WeaponDamage[playerid][50] = 330.0;  // 50 - Helicopter blades
	DP_WeaponDamage[playerid][51] = 82.5; 	// 51 - Explosion
	DP_WeaponDamage[playerid][52] = 1.0; 	// 52 - Car park (custom)
	DP_WeaponDamage[playerid][53] = 0.0; 	// 52 - Drowning
	DP_WeaponDamage[playerid][54] = 165.0;  // 52 - Splat

	DP_DeathData[playerid][dpValid] = false;
	DP_DeathData[playerid][dpKillerid] = INVALID_PLAYER_ID;
	DP_DeathData[playerid][dpReason] = WEAPON_UNARMED;

	DP_LastExplosive[playerid] = WEAPON_UNARMED;
	DP_KnifeTimer[playerid] = -1;

	DP_TeamID[playerid] = NO_TEAM;
	SetPlayerTeam(playerid, 0x255);
	return 1;
}


hook OnPlayerSpawn(playerid) 
{
	DP_DeathData[playerid][dpValid] = false;
	DP_DeathData[playerid][dpKillerid] = INVALID_PLAYER_ID;
	DP_DeathData[playerid][dpReason] = WEAPON_UNARMED;

	DP_LastExplosive[playerid] = WEAPON_UNARMED;

	KillTimer(DP_KnifeTimer[playerid]);
	DP_KnifeTimer[playerid] = -1;

	DP_SetPlayerHealth(playerid, 100.0);
	return 1;
}


hook OnPlayerDeath(playerid, killerid, reason) 
{
	if (!DP_DeathData[playerid][dpValid])
	    return 0;

	KillTimer(DP_KnifeTimer[playerid]);
	DP_KnifeTimer[playerid] = -1;

	killerid = DP_DeathData[playerid][dpKillerid];
	reason = DP_DeathData[playerid][dpReason];

	DP_DeathData[playerid][dpValid] = false;
	return 1;
}


hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) 
{
    if (newkeys & KEY_FIRE) 
    {
		new weapon = GetPlayerWeapon(playerid);

		switch (weapon) 
		{
			case WEAPON_BOMB, WEAPON_SATCHEL: 
			{
				DP_LastExplosive[playerid] = WEAPON_SATCHEL;
			}

			case WEAPON_ROCKETLAUNCHER, WEAPON_HEATSEEKER, WEAPON_GRENADE: 
			{
				DP_LastExplosive[playerid] = weapon;
			}
		}
	}
	return 1;
}


hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) 
{
	if (!DP_IsPlayerSpawned(playerid))
		return 0;


	if (hittype != BULLET_HIT_TYPE_NONE)
	{
	    new Float:max_range;

	    switch (weaponid) 
	    {
	        case WEAPON_COLT45, WEAPON_SILENCED, WEAPON_DEAGLE: 	max_range = 35.0;
			case WEAPON_SHOTGUN, WEAPON_SHOTGSPA: 					max_range = 40.0;
			case WEAPON_SAWEDOFF: 									max_range = 35.0;
			case WEAPON_UZI, WEAPON_TEC9: 							max_range = 35.0;
			case WEAPON_MP5: 										max_range = 45.0;
			case WEAPON_AK47: 										max_range = 70.0;
			case WEAPON_M4, WEAPON_VEHICLE_M4: 						max_range = 90.0;
			case WEAPON_RIFLE:	 									max_range = 100.0;
			case WEAPON_SNIPER: 									max_range = 320.0;
			case WEAPON_VEHICLE_MINIGUN, WEAPON_MINIGUN: 			max_range = 75.0;
	    }

		new 
			Float:ox, 
			Float:oy, 
			Float:oz, 
			Float:hx, 
			Float:hy, 
			Float:hz;

		GetPlayerLastShotVectors(playerid, ox, oy, oz, hx, hy, hz);

		if (_:max_range && VectorSize((ox - hx), (oy - hy), (oz - hz)) > max_range)
			return 0;
	}

	return 1;
}


hook OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
	if (damagedid == INVALID_PLAYER_ID)
		return 1;

	if (!(DP_IsPlayerSpawned(playerid) || DP_IsPlayerSpawned(damagedid)))
		return 1;

	new 
		team_playerid = GetPlayerTeam(playerid),
		team_damagedid = GetPlayerTeam(damagedid);


	if (team_playerid != NO_TEAM && team_damagedid != NO_TEAM && team_playerid == team_damagedid) 
	{
	    #if defined OnPlayerDamage
			if(!OnPlayerDamage(damagedid, playerid, amount, weaponid, bodypart))
				return 1;
		#endif

		return 1;
	}

	new 
		Float:armour, 
		Float:health = DP_PlayerHealth[playerid];


    if (weaponid == WEAPON_KNIFE) 
    {
		if (_:amount == _:0.0) 
		{
			GetPlayerArmour(damagedid, armour);

			if (DP_KnifeTimer[playerid] != -1)
				KillTimer(DP_KnifeTimer[playerid]);


			#if defined OnPlayerDamage
				if(!OnPlayerDamage(damagedid, playerid, amount, weaponid, bodypart))
					return 1;
			#endif

			DP_KnifeTimer[playerid] = SetTimerEx("DP_OnPlayerKnifed", 2200, false, "ii", damagedid, playerid);
			return 1;
		}
	}

	if (!DP_ProcessDamage(damagedid, playerid, amount, weaponid, bodypart))
		return 1;


	#if defined OnPlayerDamage
		if (OnPlayerDamage(damagedid, playerid, amount, weaponid, bodypart) == 0)
		    return 0;
	#endif

	DP_InflictDamage(damagedid, amount, false);

	health = DP_PlayerHealth[playerid];

	if (health == 0.0) 
	{
		SetPlayerHealth(playerid, 0.0);
	    DP_DeathData[damagedid][dpValid] = true;
	    DP_DeathData[damagedid][dpKillerid] = playerid;
	    DP_DeathData[damagedid][dpReason] = weaponid;
	}

	return 1;
}

hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart) 
{
	switch (weaponid) 
	{
		case WEAPON_FLAMETHROWER, WEAPON_VEHICLE, WEAPON_CARPARK, WEAPON_EXPLOSION, WEAPON_DROWN, WEAPON_COLLISION: 
		{
			new bool:explosive;

            if (weaponid == WEAPON_EXPLOSION)
            {
                explosive = true;
            
            	if (issuerid == INVALID_PLAYER_ID) 
            	{
			    	if (IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleSeat(playerid) == 0) 
			    	{
					    weaponid = WEAPON_VEHICLE_ROCKETLAUNCHER;
					} 
					else 
					{
						if (DP_LastExplosive[playerid] != WEAPON_UNARMED)
							weaponid = DP_LastExplosive[playerid];
					}
				}
				else 
				{
			    	if (IsPlayerInAnyVehicle(issuerid) && GetPlayerVehicleSeat(issuerid) == 0) 
			    	{
					    weaponid = WEAPON_VEHICLE_ROCKETLAUNCHER;
					} 
					else 
					{
						if (DP_LastExplosive[issuerid] != WEAPON_UNARMED)
							weaponid = DP_LastExplosive[issuerid];
					}
				}
			}

			#if defined OnPlayerDamage
				if(!OnPlayerDamage(playerid, issuerid, amount, weaponid, bodypart))
					return 1;
			#endif


			new 
				Float:armour, 
				Float:health = DP_PlayerHealth[playerid];

				

			GetPlayerArmour(playerid, armour);
			

			if(issuerid != INVALID_PLAYER_ID && (weaponid == WEAPON_FLAMETHROWER || weaponid == WEAPON_CARPARK || weaponid == WEAPON_VEHICLE || explosive == true)) 
			{
                new 
                	team_playerid = GetPlayerTeam(playerid),
					team_issuerid = GetPlayerTeam(issuerid);

				if (team_playerid != NO_TEAM && team_issuerid != NO_TEAM && team_playerid == team_issuerid)
					return 1;
				
                DP_InflictDamage(playerid, amount, false);
			}
			else 
			{
				armour = 0.0;
				health -= amount;
				if (health < 0.0) health = 0.0;
			}

			DP_PlayerHealth[playerid] = health;

            if (health <= 0.0) 
            {

            	SetPlayerHealth(playerid, 0.0);
			    DP_DeathData[playerid][dpValid] = true;
			    DP_DeathData[playerid][dpKillerid] = playerid;
			    DP_DeathData[playerid][dpReason] = weaponid;
			}
		}
	}

	return 1;
}

forward DP_OnPlayerKnifed(playerid, issuerid);
public DP_OnPlayerKnifed(playerid, issuerid) 
{
    SetPlayerArmour(playerid, 0.0);
	SetPlayerHealth(playerid, 0.0);

	DP_DeathData[playerid][dpValid] = true;
 	DP_DeathData[playerid][dpKillerid] = issuerid;
 	DP_DeathData[playerid][dpReason] = WEAPON_KNIFE;
}

stock DP_SetPlayerTeam(playerid, team) 
{
	if (-1 < playerid < MAX_PLAYERS) 
	    return 0;

	DP_TeamID[playerid] = team;
	return 1;
}


stock DP_GetPlayerTeam(playerid) 
{
	if (!(-1 < playerid < MAX_PLAYERS)) 
	    return 0;

	return DP_TeamID[playerid];
}


stock DP_SetPlayerHealth(playerid, Float:health)
{
	if (!(-1 < playerid < MAX_PLAYERS)) 
	    return 0;

	DP_PlayerHealth[playerid] = health;

	if (health <= 0.0) 
	{
		DP_DeathData[playerid][dpValid] = true;
	 	DP_DeathData[playerid][dpKillerid] = INVALID_PLAYER_ID;
	 	DP_DeathData[playerid][dpReason] = WEAPON_UNKNOWN;

	 	return SetPlayerHealth(playerid, 0.0);
	}

	return SetPlayerHealth(playerid, 8000000.0);
}


stock DP_GetPlayerHealth(playerid, &Float:health)
{
	if (!(-1 < playerid < MAX_PLAYERS)) 
	    return 0;

	health = DP_PlayerHealth[playerid];
	return 1;
}


stock DP_SendDeathMessage(playerid, killerid, reason) 
{
	switch (reason) 
	{
	    case WEAPON_UNKNOWN: 					reason = 47;
	    case WEAPON_CARPARK:					reason = WEAPON_VEHICLE;
	    case WEAPON_PISTOLWHIP:	 				reason = WEAPON_UNARMED;
	    case WEAPON_VEHICLE_M4: 				reason = WEAPON_M4;
	    case WEAPON_VEHICLE_MINIGUN: 			reason = WEAPON_MINIGUN;
	    case WEAPON_VEHICLE_ROCKETLAUNCHER: 	reason = WEAPON_ROCKETLAUNCHER;
	}
	return SendDeathMessage(playerid, killerid, reason);
}


stock SetPlayerWeaponDamage(playerid, weaponid, Float:amount) 
{
	if (!(-1 < playerid < MAX_PLAYERS)) 
	    return 0;

	if (!(-1 < weaponid < 54))
	    return 0;


    DP_WeaponDamage[playerid][weaponid] = amount;
	return 1;
}


stock Float:GetPlayerWeaponDamage(playerid, weaponid) 
{
	if (!(-1 < playerid < MAX_PLAYERS)) 
	    return 0;

	if (!(-1 < weaponid < 54))
	    return 0;

    return DP_WeaponDamage[playerid][weaponid];
}



static DP_IsPlayerSpawned(playerid) 
{
	if (DP_KnifeTimer[playerid] != -1)
		return 0;

	switch (GetPlayerState(playerid)) 
	{
		case PLAYER_STATE_ONFOOT .. PLAYER_STATE_PASSENGER, PLAYER_STATE_SPAWNED:
		{
			return 1;
		}
	}

	return 0;
}


/* ————————————————————————————————————————————————————————————————

HOOKS

———————————————————————————————————————————————————————————————— */	


#if defined _ALS_SetPlayerHealth
	#undef SetPlayerHealth
#else
	#define _ALS_SetPlayerHealth
#endif
#define SetPlayerHealth DP_SetPlayerHealth



#if defined _ALS_GetPlayerHealth
	#undef GetPlayerHealth
#else
	#define _ALS_GetPlayerHealth
#endif

#define GetPlayerHealth DP_GetPlayerHealth


#if defined _ALS_SetPlayerTeam
    #undef SetPlayerTeam
#else
    #define _ALS_SetPlayerTeam
#endif
#define SetPlayerTeam DP_SetPlayerTeam



#if defined _ALS_GetPlayerTeam
    #undef GetPlayerTeam
#else
    #define _ALS_GetPlayerTeam
#endif
#define GetPlayerTeam DP_GetPlayerTeam



#if defined _ALS_SendDeathMessage
    #undef SendDeathMessage
#else
    #define _ALS_SendDeathMessage
#endif
#define SendDeathMessage DP_SendDeathMessage


#undef DP_IsMelee