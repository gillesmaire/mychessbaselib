#! /bin/sh

## Some files are to be unmodified but other are to be modified. Sometimes modifications are made in wrong files.
## This script checks that original files are not modified. If yes it proposes to copy the good version.
cpy()
{
 echo cp src/$1 ../../scid5lib
}

FILES="attacks.h attacks.h codec.h codec_scid4.h date.h filebuf.h hash.h movelist.h namebase.h position.h sqmove.h board_def.h codec_memory.h codec_scid5.h dbasepool.h fullmove.h hfilter.h matsig.h  movetree.h pgn_encode.h scidbase.h stored.h bytebuf.h codec_pgn.h common.h dstring.h game.h indexentry.h naglatex.h  pgn_lexer.h searchtournaments.h textbuf.h codec_proxy.h containers.h error.h gameview.h index.h movegen.h nagtext.h sortcache.h tree.h codec_scid4.cpp dbasepool.cpp matsig.cpp position.cpp sortcache.cpp scidbase.cpp stored.cpp game.cpp misc.cpp textbuf.cpp"

for f in $FILES
do
 cmp /usr/local/src/scid-code/src/$f ../$f
 if  [ $? -ne 0 ]
 then
    diff /usr/local/src/scid-code/src/$f ../$f
    echo -n  "Copier ? (Y/N)"
    read rep
    if [ "$rep" = "Y" ] || [ "$rep" = "y" ]
    then
        cp /usr/local/src/scid-code/src/$f ../$f
    fi
 fi
# sudo chattr +i ../$f
done

