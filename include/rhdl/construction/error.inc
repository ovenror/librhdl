E_NO_ERROR = 0,

// C API
E_UNKNOWN_STRUCT,
E_WRONG_STRUCT_TYPE,

// Incompatible Interfaces -> InterfaceCompatException
E_OPEN_TO_OPEN,
E_INCOMPATIBLE_DIRECTIONS,
E_DIRECTION_OPPOSES_OPERATOR,
E_DIFFERENT_TYPES,
E_DIFFERENT_NUMBER_OF_COMPONENTS,

E_INTERFACES_NOT_COMPATIBLE,

// Other connection errors
E_COMPATIBLE_INTERFACES_NOT_FOUND,
E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES,
E_ILLEGAL_RECONNECTION,
E_ILLEGAL_PASSTHROUGH,

E_ALREADY_CONNECTED_TO_OPEN,
E_UNKNOWN_CONNECTION_FAILURE,

E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY,

// Structure finalization
E_NETLIST_CONTAINS_CYCLES,

// Lookup errors and name conflicts
E_CANNOT_GET_ANONYMOUS_INTERFACE,
E_NO_SUCH_INTERFACE,
E_NO_SUCH_ENTITY,
E_ENTITY_EXISTS,

// Simulator construction
E_WRONG_NAME,
E_WRONG_TYPE