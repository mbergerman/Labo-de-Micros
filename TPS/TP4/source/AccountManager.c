/***************************************************************************//**
  @file     AccountManager.h
  @brief    Manager of accounts in the database
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "AccountManager.h"
#include <stdio.h>

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static User user_data_base[DATA_BASE_MAX];
static int users_in_db = 0;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initDataBase()
{
	User master_user;
	master_user.ID = MASTER_ID;
	master_user.PIN = MASTER_PIN;
	master_user.admin = true;
	master_user.error_counter = 0;
	master_user.office_floor = 0;
	master_user.inside_building = false;
	user_data_base[users_in_db] = master_user;
	users_in_db++;

	// Tarjetas que teníamos a mano:
	dbAddNewUser(master_user, 22747207, 800, false, 1);
	dbAddNewUser(master_user, 38966311, 60831, false, 1);	// Tarjeta ITBA M.Bergerman

	// Test user:
	dbAddNewUser(master_user, 10000000, 10090, false, 2);	// Ejemplo
}

//Devuelve si el ID está en la BD
bool dbCheckID(uint32_t ID_buffered)
{
	bool check = false;
	for (uint8_t i = 0; (i < users_in_db) && !check; i++) {
		uint32_t id_db = user_data_base[i].ID;
		if (id_db == ID_buffered) {
			check = true;
		}
	}
	return check;
}

//Devuelve el User de un determinado ID
User* dbUserSearch(uint32_t ID_to_search)
{
	for (uint8_t i = 0; (i < users_in_db); i++) {
		uint32_t id_db = user_data_base[i].ID;
		if (id_db == ID_to_search) {
			return &user_data_base[i];
		}
	}

	return NULL;	// No se encontró el user (no debería pasar ya que antes se llama a id_check
}

bool dbAddNewUser(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u, uint8_t floor){
	bool done = false;
	if(admin_user.admin && !dbCheckID(ID_u)){
		User new_user;
		new_user.ID = ID_u;
		new_user.PIN = PIN_u;
		new_user.admin = admin_u;
		new_user.error_counter = 0;
		new_user.office_floor = floor;
		new_user.inside_building = false;
		user_data_base[users_in_db] = new_user;
		users_in_db++;
		done = true;
	}
	return done;
}

bool dbRemoveUser(User admin_user, uint32_t ID_u){
	bool found = false;
    for (uint8_t i = 0; admin_user.admin && (i < users_in_db) && !found ; i++) {
		uint32_t id_db = user_data_base[i].ID;
		if (id_db == ID_u) {
			user_data_base[i] = user_data_base[users_in_db-1];
			found = true;
		}
	}
	return found;
}

bool dbUserBlocked(User* u){
	return (u->error_counter >= MAX_ERRORS);
}


bool dbToggleInsideBuilding(User* u){
	u->inside_building = !u->inside_building;
	return u->inside_building;
}


