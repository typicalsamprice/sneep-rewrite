// -*- mode: c++; -*-
#pragma once
#include "position.h"
#include "types.h"
#include <string>

Move parse_move(std::string move, const Position &pos);
