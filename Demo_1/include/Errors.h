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

#endif
