#pragma once

namespace eone
{
namespace particle3d
{

enum
{
	LK_CENTER = 0,
	LK_OFFSET,
	LK_MIN,
	LK_MAX,
	LK_RANGE,
	LK_SPEED,
	LK_START,
	LK_END,
	LK_RADIUS,
	LK_HEIGHT,

	LK_STATIC_MODE,
	LK_NAME,
	LK_CONST_COUNT,
	LK_STATE,
	LK_LOOP,
	LK_LOCAL_DRAW,
	LK_COUNT,
	LK_EMISSION_TIME,
	LK_LIFE,
	LK_HORI,
	LK_VERT,
	LK_RADIAL_SPEED,
	LK_TANGENTIAL_SPEED,
	LK_ANGULAR_SPEED,
	LK_DISTURBANCE,
	LK_GRAVITY,
	LK_LINEAR_ACC,
	LK_INERTIA,
	LK_FADEOUT_TIME,
	LK_ADDITIVE_BLAND,
	LK_START_POSITION,
	LK_ORIENT_MOVEMENT,
	LK_ORIENT_PARENT,
	LK_BLEND,
	LK_BLEND_NULL,
	LK_BLEND_ADD,
	LK_BLEND_SUBTRACT,

	LK_NO_GROUND,
	LK_GROUND_WITH_BOUNCE,
	LK_GROUND_NO_BOUNCE,

	LK_REMOVE,
	LK_REMOVE_ALL,
	LK_COMPONENTS,

	LK_SCALE,
	LK_ROTATE,
	LK_COL_MUL,
	LK_COL_ADD,
	LK_ALPHA,
};

static const char* EN[] =
{
	"center",
	"offset",
	"min",
	"max",
	"range (pixel)",
	"speed (pixel)",
	"start",
	"end",
	"radius",
	"height",

	"Static Mode",
	"Name",
	"Const Count",
	"State",
	"Loop",
	"Local Draw",
	"Count",
	"Emission Time (ms)",
	"Life (ms)",
	"Horizontal (deg)",
	"Vertical (deg)",
	"Radial Speed (pixel)",
	"Tangential Speed (pixel)",
	"Angular Speed (degree)",
	"Disturbance",
	"Gravity (pixel)",
	"Linear Acc (pixel)",
	"Inertia",
	"Fadeout Time (ms)",
	"Additive Blend",
	"Start Position (pixel)",
	"Orient to Movement",
	"Orient to Parent",
	"Blend",
	"Null",
	"Add",
	"Subtract",

	"No Ground",
	"Ground with Bounce",
	"Ground without Bounce",

	"Remove",
	"Remove All",
	"Components",

	"Scale (%)",
	"Rotate (deg)",
	"Col Mul",
	"Col Add",
	"Alpha",
};

static const char* CN[] =
{
	"中心",
	"偏移",
	"最小",
	"最大",
	"幅度",
	"速度",
	"起始",
	"结束",
	"半径",
	"高度",

	"静态模式",
	"名字",
	"固定数量",
	"播放状态",
	"连续播放",
	"局部坐标",
	"数量",
	"发射周期(毫秒)",
	"生命周期(毫秒)",
	"水平发射角度",
	"竖直发射角度",
	"径向速度",
	"切向速度",
	"角速度",
	"扰动",
	"重力",
	"线性加速度",
	"转动惯量",
	"消逝时间",
	"叠加渲染",
	"起始位置",
	"朝向运动方向",
	"朝向父节点方向",
	"混合",
	"无混合",
	"相加",
	"相减",

	"无地面",
	"弹性地面",
	"非弹性地面",

	"删除",
	"删除全部",
	"组件",

	"缩放(%)",
	"旋转",
	"乘色",
	"加色",
	"透明度",
};

static const char** LANG = CN;

}
}