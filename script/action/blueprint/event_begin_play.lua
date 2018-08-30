-- create node event_begin_play

require "moon.bp"

local node = moon.bp.new_node("bp_event_begin_play")
if node then
	node:set_pos(moon.mouse.get_position())
end