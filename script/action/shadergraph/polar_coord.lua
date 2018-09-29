-- create node PolarCoordinates

require "moon.sg"

local node = moon.sg.new_node("sg_polar_coord")
if node then
	node:set_pos(moon.mouse.get_position())
end