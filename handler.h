#ifndef HANDLER_H
#define HANDLER_H

#include "commands.h"


class handler : public commands
{
public:
    handler(commands *cmd = 0);
};

#endif // HANDLER_H
