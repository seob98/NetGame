// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>//#include <C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/core/inc/fmod.h>

// C 런타임 헤더 파일입니다.
//#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <algorithm>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <math.h>

#include "extern.h"
#include "define.h"
#include "functor.h"
#include "struct.h"

#include "BitMap.h"
#include "BmpMgr.h"
#include "SoundMgr.h"

#include "Block.h"
#include "Player.h"
#include "Ballon.h"
#include "Obstacle.h"
#include "WaterStream.h"
#include "Item.h"
#include "UI.h"

#include "protocol.h"
#include "Common.h"



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
