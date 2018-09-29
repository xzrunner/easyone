-- create node NormalBlend

require "moon.sg"

local node = moon.sg.new_node("sg_normal_blend")
if node then
	node:set_pos(moon.mouse.get_position())
end