# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show
# Create empty queue
new
# See how long it is
size
# Fill it with some values.  First at the head
ih dolphin
ih bear
ih gerbil
# Now at the tail
it meerkat
it bear
# Reverse it
reverse
# See how long it is
size
# Delete queue.  Goes back to a NULL queue.
free

new
ih 1
ih 3
ih 6
ih 2
ih 3
ih 7
# 00000000
option descend 0
# do_sort
sort
# 00000000
option descend 1
# do_sort
sort

free
# Exit program
quit
