#include <stdint.h>
#include <stdbool.h>

#define DATA_BASE_MAX 100
#define ID_SIZE 8
#define PIN_SIZE_MAX 6
#define LATCH_TIME_MS 5000
#define MASTER_ID 12345678
#define MASTER_PIN 12345


typedef struct Users {
	uint32_t ID;
	uint32_t PIN;
	bool admin;
} User;

void init_data_base();

//Devuelve si el ID está en la BD
bool id_check(uint32_t ID_buffered);

//Devuelve el User de un determinado ID
User user_search(uint32_t ID_to_search);

//Se puede agregar solo si hay un usuario Admin que lo valide
bool add_to_data_base(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u);

//Se puede eliminar solo si hay un usuario Admin que lo valide
bool delete_from_data_base(User admin_user, uint32_t ID_u);

bool edit_PIN(uint32_t user_to_change, uint32_t PIN_u);

