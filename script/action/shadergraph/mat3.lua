-- create node Matrix3

require "moon.sg"

local node = moon.sg.new_node("sg_mat3")
if node then
	node:set_pos(moon.mouse.get_position())
end