import subprocess
import os

cwd = os.getcwd()
os.environ['LD_LIBRARY_PATH'] = cwd

programs = [
'hello-bin',
'02-sdl-dynamic',
'02-sdl-static',
'03-sdl-loop',
'03-sdl-loop-to-engine',
'game-03-3',
'game-04-1',
'game-04-2',
'game-05-1',
'game-05-2',
'game-05-3',
'game-06-1',
'game-06-2',
'game-06-3',
'sound_test-07-1',
'game-07-2',
'game-08-1',
'engine-08-2',
'engine-08-3',
'engine-09-1',
'engine-10-1'
]

for programm_name in programs:
    dir = cwd + "/../../tests/" + programm_name
    print("starting: ", programm_name, " in dir: ", dir)

    result = subprocess.run([cwd + '/' + programm_name], stdout=subprocess.PIPE, cwd=cwd)
    if result.returncode != 0:
        print("error: \nargs: ", result.args, "\nstdout: ", result.stdout, "\nreturncode: {}", result.returncode)
        break


