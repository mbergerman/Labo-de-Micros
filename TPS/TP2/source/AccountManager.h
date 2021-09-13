#include <stdint.h>
#include <stdbool.h>

#define DATA_BASE_MAX 100
#define ID_SIZE 8
#define PIN_SIZE_MAX 6
#define MASTER_ID 12345678
#define MASTER_PIN 12345
#define MAX_ERRORS 3

typedef struct Users {
	uint32_t ID;
	uint32_t PIN;
	bool admin;
	uint8_t error_counter;
} User;

static void init_data_base();

//Devuelve si el ID está en la BD
static bool id_check(uint32_t ID_buffered);

//Devuelve el User de un determinado ID. PRIMERO HAY QUE HACER UN ID CHECK
static User* user_search(uint32_t ID_to_search);

//Se puede agregar solo si hay un usuario Admin que lo valide
static bool add_to_data_base(User admin_user, uint32_t ID_u, uint32_t PIN_u, bool admin_u);

//Se puede eliminar solo si hay un usuario Admin que lo valide
static bool delete_from_data_base(User admin_user, uint32_t ID_u);

static bool is_blocked(User* u);