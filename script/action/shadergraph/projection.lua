-- create node Projection

require "moon.sg"

local node = moon.sg.new_node("sg_projection")
if node then
	node:set_pos(moon.mouse.get_position())
end