-- create node from file with cursor position

require "moon.bp"

local node = moon.bp.new_node()
if node then
	node:set_pos(moon.mouse.get_position())
end