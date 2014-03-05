namespace Retro {

public class Runner {
	private Runnable runnable;
	private uint? loop;
	
	public Runner (Runnable runnable) {
		this.runnable = runnable;
		loop = null;
	}
	
	~Runner () {
		stop ();
	}
	
	public void start () {
		loop = Timeout.add (100/6, run);
	}
	
	public void stop () {
		if (loop != null) {
			Source.remove (loop);
		}
		
		loop = null;
	}
	
	public bool run () {
		if (runnable != null && loop != null) {
			runnable.run ();
			return true;
		}
		
		return false;
	}
}

}

