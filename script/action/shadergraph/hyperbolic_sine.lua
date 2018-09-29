-- create node HyperbolicSine

require "moon.sg"

local node = moon.sg.new_node("sg_hyperbolic_sine")
if node then
	node:set_pos(moon.mouse.get_position())
end