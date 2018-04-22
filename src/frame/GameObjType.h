#pragma once

#include <ee0/GameObj.h>

namespace eone
{

enum GameObjType
{
	GAME_OBJ_UNKNOWN = 0,

	GAME_OBJ_IMAGE = 1,
	GAME_OBJ_TEXT,
	GAME_OBJ_MASK,
	GAME_OBJ_MESH,
	GAME_OBJ_SCALE9,
	GAME_OBJ_ANIM,
	GAME_OBJ_PARTICLE3D,
	GAME_OBJ_SCENE2D,
	GAME_OBJ_SCENE3D,
};

GameObjType GetObjType(const ee0::GameObj& obj);

}