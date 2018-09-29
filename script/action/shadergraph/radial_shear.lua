-- create node RadialShear

require "moon.sg"

local node = moon.sg.new_node("sg_radial_shear")
if node then
	node:set_pos(moon.mouse.get_position())
end