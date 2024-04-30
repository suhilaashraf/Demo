#ifndef ERRORS_H
#define ERRORS_H

typedef enum{
    OK,
    NOK,
    ParameterError,
    ConfigurationError,
	Clock_SourceError,
    NULL_PointerError
}ErrorStatus_t;


typedef enum
{
	/**
	 *@brief : Everything Ok, Function had Performed Correctly.
	 */
	Status_enumOk ,
	/**
	 *@brief :NOT oK ERROR
	 */
	Status_enumNotOk ,
	/**
	 *@brief :Wrong Input error
	 */
	Status_enumParameterError,
	/**
	 *@brief :Configuration error
	 */
	Status_enumCFGError,
	/**
	 *@brief :Null Pointer error
	 */
	Status_enumNULLPointer,

}Error_enumStatus_t;

#endif

