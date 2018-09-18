-- create node Vector4

require "moon.sg"

local node = moon.sg.new_node("sg_vec4")
if node then
	node:set_pos(moon.mouse.get_position())
end