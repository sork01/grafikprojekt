
def intersection(b, r):
    tmin = -999999
    tmax = 999999;
    
    if r['x'] != 0.0:
        t1 = (b['xmin'] - r['x0'])/r['x'] # how far to travel on ray until hit xmin
        t2 = (b['xmax'] - r['x0'])/r['x'] # how far to travel on ray until hit xmax
        
        tmin = max(tmin, min(t1, t2)) # tmin is ray travel distance until enter x-range
        tmax = min(tmax, max(t1, t2)) # tmax is ray travel distance until leave x-range
        
        print(tmin)
        print(tmin * r['x'])
        print(tmax)
        print(tmax * r['x'])
        print()
    
    
    if r['y'] != 0.0:
        t1 = (b['ymin'] - r['y0'])/r['y'] # how far to travel on ray until hit ymin
        t2 = (b['ymax'] - r['y0'])/r['y'] # how far to travel on ray until hit ymax
        
        tmin = max(tmin, min(t1, t2)) # if distance needed to enter y-range is larger than
                                      # distance needed to enter x-range, update tmin
        
        print(tmin)
        print(tmin*r['y'])
        
        tmax = min(tmax, max(t1, t2)) # if distance needed to leave y-range is smaller
                                      # than distance needed to leave x-range, update tmax
        
        # so in the example 
        #
        # b = {'xmin':1.0, 'xmax':2.0, 'ymin':1.0, 'ymax':2.0}
        # r = {'x':3.0, 'y':1.0, 'x0':0.0, 'y0':0.0}
        #
        # we first calculate (for x component) tmin=0.333, tmax=0.666 
        # and then find that we need to travel 1.0*r to hit y-range
        # so update tmin=1.0
        # and then find that we need to travel 2.0*r to leave y-range
        # so don't update tmax
    
    # so tmin = minimum ray distance we HAVE TO travel to reach range(s)
    #    tmax = maximum ray distance we are ALLOWED TO travel, because we'll exit some range after this
    
    return tmax >= tmin # if the distance needed to leave the range is smaller than
                         # the distance needed to enter it

b = {'xmin':1.0, 'xmax':2.0, 'ymin':1.0, 'ymax':2.0}
r = {'x':3.0, 'y':1.0, 'x0':0.0, 'y0':0.0}

print(intersection(b,r))
