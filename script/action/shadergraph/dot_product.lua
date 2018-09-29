-- create node DotProduct

require "moon.sg"

local node = moon.sg.new_node("sg_dot_product")
if node then
	node:set_pos(moon.mouse.get_position())
end