cmake_minimum_required (VERSION 2.6)
set(CMAKE_INSTALL_PREFIX /usr/local)
project (magma)
add_executable (magma src/act_clan.c  src/act_obj.c  src/const.c  src/fraglist.c  src/justice.c  src/necrospells.c  src/songs.c     src/ssm.c     src/traps.c
src/act_comm.c  src/act_wiz.c  src/db.c     src/gamble.c    src/magic.c    src/prog_comm.c    src/spec_mob.c  src/string.c  src/update.c
src/act_info.c  src/bit.c      src/event.c  src/handler.c   src/map.c      src/progs.c        src/spec_obj.c  src/tables.c  src/vehicle.c
src/act_move.c  src/comm.c     src/fight.c  src/interp.c    src/mem.c      src/save.c         src/spells.c    src/track.c   src/wiznet.c)
target_link_libraries(magma m)
install (TARGETS magma DESTINATION bin PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_READ OWNER_EXECUTE OWNER_READ)
install (DIRECTORY area backup clans classes editors mobprogs objprogs player sys DESTINATION share/magma
         PATTERN "*"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE GROUP_WRITE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
