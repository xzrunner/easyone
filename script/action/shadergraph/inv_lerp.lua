-- create node InverseLerp

require "moon.sg"

local node = moon.sg.new_node("sg_inverse_lerp")
if node then
	node:set_pos(moon.mouse.get_position())
end