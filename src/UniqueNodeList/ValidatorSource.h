#ifndef VALIDATOR_SOURCE_H
#define VALIDATOR_SOURCE_H

typedef enum {
    vsConfig = 'C',	// newcoind.cfg
    vsInbound = 'I',
    vsManual = 'M',
    vsReferral = 'R',
    vsTold = 'T',
    vsValidator = 'V',	// validators.txt
    vsWeb = 'W',
} validatorSource;

#endif // VALIDATOR_SOURCE_H
