    # ---- the Blueprints, each after those it calls ----
    # A Blueprint's conversion is a function of what it is given (job_of): its files, of a header of the engine
    # the functions the program calls, and what the Blueprints it may call have written - their tables and the
    # pieces their tests hand on. It is converted in rounds: every round takes what is known at its start, for
    # every Blueprint of the round alike, converts them all at once and puts what they give in place in the
    # order of their names - so what is written does not hang on how many convert at once, nor on which is
    # done first. A Blueprint is converted again when what it is given has changed since it was converted, and
    # only then (the same given, the same written); in a round, those that nothing they may call still waits to
    # be converted for - except those that call them back: Blueprints that call each other (the parts of one
    # Blueprint do) are converted together, round after round, each with what the others gave in the round
    # before, until none of them is given anything new. What they would call of each other that is not written
    # then waits for good, as a function that calls itself. Nothing is kept from one run to the next.
    tables, exports, results = {}, {}, {}
    given = {}                              # a Blueprint -> what it was given when it was converted last
    gave = collections.defaultdict(list)    # a Blueprint -> what it has given (its table, its exports), each once
    version = {}                            # a Blueprint -> which of those it gives now (an index into gave)
    met, rounds = set(), 0

    def asked_of(u, needs):
        """What a Blueprint is given, told by what it is made of: its files, the functions to write, and which
        of what they gave the Blueprints it may call give now."""
        return (tuple(sorted(unit_files[u])), None if u in program else tuple(sorted(wanted[u])),
                tuple((d, version.get(d)) for d in sorted(needs[u])))

    while True:
        needs = {u: need_of(u) for u in deps}
        asks = {u: asked_of(u, needs) for u in deps}
        new = {u for u in deps if given.get(u) != asks[u]}
        if not new:
            break
        # (the same state twice: the rounds would go round for ever - what the Blueprints named give changes
        # what they are given, and that changes back)
        state = (tuple(sorted(asks.items())), tuple(sorted(given.items())))
        if state in met:
            raise SystemExit("these Blueprints give, round after round, what they gave before (what one writes takes from the other what "
                             "it writes): " + ", ".join(sorted(new)))
        met.add(state)
        now = sorted(u for u in new if all(d not in new or u in needs[d] for d in needs[u]))
        rounds += 1
        print(f"round {rounds}: {len(now)} Blueprints", flush=True)
        jobs = [job_of(u) for u in now]   # (all of the round's from the same state, before any of them is done)
        got = dict(zip(now, pool.map(convert_unit, jobs)))
        for u in now:
            r = got[u]
            given[u] = asks[u]
            results[u] = r
            tables[u], exports[u] = r["table"], r["exports"]
            value = (r["table"], r["exports"])
            if value not in gave[u]:
                gave[u].append(value)
            version[u] = gave[u].index(value)
        for u in now:
            # What it would call of a Blueprint that has no such function written: that Blueprint is one it calls
            # (the plan may not have known it, or not the call) - and of a header of the engine, one more
            # function of it is written.
            for d, key in got[u]["missing"]:
                if key not in functions or d == u:
                    continue
                if d not in deps:
                    deps[d] = set()
                    unit_files[d].add(functions[key]["file"])
                if d not in program and key not in tables.get(d, {}):
                    wanted[d].add(key)
                deps[u].add(d)

    for u in sorted(results):
        r = results[u]
        sys.stderr.write(r["errors"])
        print(f"{u:28} {r['done']:4} / {r['total']:4} functions   {r['done_lines']:6} / {r['total_lines']:6} lines" + ("" if u in program else "   (what the program calls of it)"), flush=True)
        if os.environ.get("FUNCTIONS"):
            for line in r["said"]:
                print(line)
    pool.shutdown()
