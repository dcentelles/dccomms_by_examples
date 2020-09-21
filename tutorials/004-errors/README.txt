
## CONFIGURATON TO SEE PROPAGATION ERRORS DUE TO LINK DISTANCE

- Node 0:
	../../build/custom_node --node-name bluerov0-dev --num-packets 0

- Node 1:
	../../build/custom_node --node-name bluerov1-dev --dst-mac 1 --payload-size 50 --data-rate 400


## CONFIGURATON TO SEE PROPAGATION ERRORS + TX FIFO DROPS

- Node 0:
	../../build/custom_node --node-name bluerov0-dev --num-packets 0

- Node 1:
	../../build/custom_node --node-name bluerov1-dev --dst-mac 1 --payload-size 50 --data-rate 800


## CONFIGURATON TO SEE PROPAGATION ERRORS + TX FIFO DROPS + COLLISIONS

- Node 0:
	../../build/custom_node --node-name bluerov0-dev --dst-mac 2 --payload-size 100 --data-rate 300

- Node 1:
	../../build/custom_node --node-name bluerov1-dev --dst-mac 1 --payload-size 50 --data-rate 500
