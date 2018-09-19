-- create node Polygon

require "moon.sg"

local node = moon.sg.new_node("sg_polygon")
if node then
	node:set_pos(moon.mouse.get_position())
end