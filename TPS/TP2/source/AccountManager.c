#include "AccountManager.h"


static User user_data_base[DATA_BASE_MAX];
static int users_in_db = 0;



void init_data_base()
{
	User master_user;
	master_user.ID = 12345678;
	master_user.PIN = 12345;
	master_user.admin = true;
	master_user.error_counter = 0;
	user_data_base[users_in_db] = master_user;
	users_in_db++;

	add_to_data_base(master_user, 22747207, 800, false);
}

//Devuelve si el ID está en la BD
bool id_check(uint32_t ID_buffered)
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
User user_search(uint32_t ID_to_search)
{
	User result;
	bool found = false;
	for (uint8_t i = 0; (i < users_in_db) && !found; i++) {
		uint32_t id_db = user_data_base[i].ID;
		if (id_db == ID_to_search) {
			found = true;
			result = user_data_base[i];
		}
	}
	return result;
}

bool add_to_data_base(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u)
{	
	bool done = false;
	if(admin_user.admin && !id_check(ID_u)){
		User new_user;
		new_user.ID = ID_u;
		new_user.PIN = PIN_u;
		new_user.admin = admin_u;
		new_user.error_counter = 0;
		user_data_base[users_in_db] = new_user;
		users_in_db++;
		done = true;
	}
	return done;
}

bool delete_from_data_base(User admin_user, uint32_t ID_u){
	bool found = false;
	if(admin_user.admin){
	for (uint8_t i = 0; i < users_in_db && !found ; i++){
		if(user_data_base[i].ID == ID_u){
			user_data_base[i] = user_data_base[users_in_db-1];
			users_in_db = users_in_db-1; 
			found = true;
		}
	}
}
	return found;
}


bool edit_PIN(uint32_t user_to_change, uint32_t new_PIN){
	bool found = false;
	if(id_check(user_to_change)){
		for (uint8_t i = 0; (i < users_in_db) && !found; i++) {
		uint32_t id_db = user_data_base[i].ID;
		if (id_db == user_to_change) {
			found = true;
			user_data_base[i].PIN = new_PIN;
		    }
	    }
	}
	return found;
}

void unblock_user(User u){
	u.error_counter = 0;
}

bool is_blocked(User u){
	return u.error_counter >= 3;
}
