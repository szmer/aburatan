CC=g++
CPPFLAGS=-std=c++98 -Wall
LFLAGS=-lcurses
NAME=aburatan

all: aburatan

aburatan: action.cpp action.hpp actor.cpp actor.hpp ammo.cpp ammo.hpp attack.cpp attack.hpp bagbox.cpp bagbox.hpp book.cpp book.hpp catalog.cpp catalog.hpp clothing.cpp clothing.hpp cloud.cpp cloud.hpp compiler.cpp compiler.hpp config.hpp counted.cpp counted.hpp creature.cpp creature.hpp crlist.cpp crlist.hpp define.hpp dialog.cpp dialog.hpp diceroll.cpp diceroll.hpp dir.cpp dir.hpp door.cpp door.hpp dun_test.cpp dun_test.hpp enum.cpp enum.hpp error.cpp error.hpp event.cpp event.hpp feature.cpp feature.hpp features.cpp features.hpp file.cpp file.hpp fluid.cpp fluid.hpp food.cpp food.hpp function.cpp function.hpp global.cpp global.hpp grammar.cpp grammar.hpp input.cpp input.hpp item.cpp item.hpp itemlist.cpp itemlist.hpp level.cpp level.hpp list.cpp list.hpp main.cpp main.hpp map.cpp map.hpp menu.cpp menu.hpp message.cpp message.hpp misc.cpp misc.hpp money.cpp money.hpp monster.cpp monster.hpp mtint.hpp name.cpp name.hpp path.cpp path.hpp player.cpp player.hpp playerui.cpp playerui.hpp point.cpp point.hpp quantity.cpp quantity.hpp quest.cpp quest.hpp random.cpp random.hpp rawtargt.cpp rawtargt.hpp rectngle.cpp rectngle.hpp resource.cpp resource.hpp scope.cpp scope.hpp serial.cpp serial.hpp skill.cpp skill.hpp sorted.cpp sorted.hpp stairs.cpp stairs.hpp standard.cpp standard.hpp stats.cpp stats.hpp string.cpp string.hpp target.cpp target.hpp temp.cpp temp.hpp terrain.cpp terrain.hpp tile.cpp tile.hpp timeline.cpp timeline.hpp trap.cpp trap.hpp types.hpp unique.cpp unique.hpp viewable.cpp viewable.hpp visuals.cpp visuals.hpp weapon.cpp weapon.hpp widget.cpp widget.hpp
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $(NAME) main.cpp
