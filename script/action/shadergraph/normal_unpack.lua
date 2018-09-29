-- create node NormalUnpack

require "moon.sg"

local node = moon.sg.new_node("sg_normal_unpack")
if node then
	node:set_pos(moon.mouse.get_position())
end