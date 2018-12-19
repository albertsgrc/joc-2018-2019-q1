{ execSync, exec } = require('child_process')
util = require 'util'
queue = require 'queue'

exec = util.promisify(exec)

N_TESTS = 10


arr =
  max: (array) ->
    Math.max.apply null, array
  min: (array) ->
    Math.min.apply null, array
  range: (array) ->
    arr.max(array) - arr.min(array)
  midrange: (array) ->
    arr.range(array) / 2
  sum: (array) ->
    num = 0
    i = 0
    l = array.length
    while i < l
      num += array[i]
      i++
    num
  mean: (array) ->
    arr.sum(array) / array.length
  median: (array) ->
    array.sort (a, b) ->
      a - b
    mid = array.length / 2
    if mid % 1 then array[mid - 0.5] else (array[mid - 1] + array[mid]) / 2
  modes: (array) ->
    if !array.length
      return []
    modeMap = {}
    maxCount = 0
    modes = []
    array.forEach (val) ->
      if !modeMap[val]
        modeMap[val] = 1
      else
        modeMap[val]++
      if modeMap[val] > maxCount
        modes = [ val ]
        maxCount = modeMap[val]
      else if modeMap[val] == maxCount
        modes.push val
        maxCount = modeMap[val]
      return
    modes
  variance: (array) ->
    mean = arr.mean(array)
    arr.mean array.map((num) ->
      (num - mean) ** 2
    )
  standardDeviation: (array) ->
    Math.sqrt arr.variance(array)
  meanAbsoluteDeviation: (array) ->
    mean = arr.mean(array)
    arr.mean array.map((num) ->
      Math.abs num - mean
    )
  zScores: (array) ->
    mean = arr.mean(array)
    standardDeviation = arr.standardDeviation(array)
    array.map (num) ->
      (num - mean) / standardDeviation

e = (cmd) ->
  execSync(cmd, { stdio: 'inherit' })


e "cp AIQuim.cc AITest.cc"
e "sed -i '' -- 's/\#define IS_DEBUG 1/\#define IS_DEBUG 0/g' AITest.cc"
e "sed -i '' -- \"s/\#define PLAYER_NAME Quim/\#define PLAYER_NAME Test/g\" AITest.cc"
e "make"

shuffle = (a) ->
  j = undefined
  x = undefined
  i = undefined
  i = a.length - 1
  while i > 0
    j = Math.floor(Math.random() * (i + 1))
    x = a[i]
    a[i] = a[j]
    a[j] = x
    i--
  a


test = (seed, against) ->
  players = shuffle([ 'Test', against, 'Dummy', 'Dummy'])
  score = {}
  cmd = "./Game #{players.join(' ')} -s #{seed} -i default.cnf -o result.res"

  console.log(cmd)

  try
    { stderr } = await exec cmd
  catch error
    console.error(error)
    process.exit 1

  result = stderr.toString().split('\n')[-7..-4]

  result.forEach((s, i) ->
    score[players[i]] = parseInt(s.split(' ')[-1..][0])
  )

  score.Test - score[against]

printResults = (results) ->
  result = {}

  for prop, fn of arr when prop in [ 'max', 'min', 'mean', 'median', 'midrange', 'standardDeviation' ]
    result[prop] = fn(results)

  console.log(result)

do main = ->
  q = queue({ concurrency: 16, autostart: true })

  results = []

  against = process.argv[2] ? "Previous"
  console.log "Evaluating against #{against}"

  for i in [0...N_TESTS]
    seed = Math.round(Math.random()*20000000)
    do (seed) ->
      q.push(->
          results.push await test(seed, against)
          if results.length is N_TESTS
            printResults(results)
      )