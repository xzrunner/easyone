-- create node ReciprocalSquareRoot

require "moon.sg"

local node = moon.sg.new_node("sg_reciprocal_square_root")
if node then
	node:set_pos(moon.mouse.get_position())
end