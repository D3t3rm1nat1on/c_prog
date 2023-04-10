create N procs "./cond_var <1-N>" (from N to 2)

```
#!/bin/bash
for i in {20..1}
do
./cond_var $i 1>$i.log 2>&1 &
done
```
