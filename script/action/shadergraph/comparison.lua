-- create node Comparison

require "moon.sg"

local node = moon.sg.new_node("sg_comparison")
if node then
	node:set_pos(moon.mouse.get_position())
end