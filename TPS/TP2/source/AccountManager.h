/***************************************************************************//**
  @file     AccountManager.h
  @brief    Manager of accounts in the database
  @author   Grupo 1
 ******************************************************************************/

#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DATA_BASE_MAX 100
#define ID_SIZE 8
#define PIN_SIZE_MAX 6
#define MASTER_ID 12345678
#define MASTER_PIN 12345
#define MAX_ERRORS 3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct Users {
	uint32_t ID;
	uint32_t PIN;
	bool admin;
	uint8_t error_counter;
} User;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void initDataBase();

//Devuelve si el ID está en la BD
bool dbCheckID(uint32_t ID_buffered);

//Devuelve el User de un determinado ID. PRIMERO HAY QUE HACER UN ID CHECK
User* dbUserSearch(uint32_t ID_to_search);

//Se puede agregar solo si hay un usuario Admin que lo valide
bool dbAddNewUser(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u);

//Se puede eliminar solo si hay un usuario Admin que lo valide
bool dbRemoveUser(User admin_user, uint32_t ID_u);

bool dbUserBlocked(User* u);

/*******************************************************************************
 ******************************************************************************/

#endif // _ACCOUNT_MANAGER_H_
