#include "stdafx.h"
#include "CCMatrix.h"

static float _matIdentity[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
} ;

const CCMatrix CCMatrix::IDENTITY(_matIdentity);
