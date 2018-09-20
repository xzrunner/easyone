-- create node Rectangle

require "moon.sg"

local node = moon.sg.new_node("sg_rectangle")
if node then
	node:set_pos(moon.mouse.get_position())
end