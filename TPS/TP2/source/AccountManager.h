#include <stdint.h>
#include <stdbool.h>

#define DATA_BASE_MAX 100
#define ID_SIZE 8
#define PIN_SIZE_MAX 6
#define LATCH_TIME_MS 5000


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

bool add_to_data_base(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u);

