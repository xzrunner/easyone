-- create node Tangent

require "moon.sg"

local node = moon.sg.new_node("sg_tangent")
if node then
	node:set_pos(moon.mouse.get_position())
end