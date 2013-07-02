>greet_prog 100~
if ispc($n)
	tell $n Leave this place.  Now.
endif
~
>fight_prog 20~
say You will make a fine pet, I think.
if rand(50)
	say Try harder, little one.
else
	say Your death will make me stronger.
endif
~
>death_prog 100~
if rand(50)
	mpechoaround $i $I says 'This is far from the end.'
else
	mpechoaround $i $I says 'I will have vengeance.'
endif
~
|
