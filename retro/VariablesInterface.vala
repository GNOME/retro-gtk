namespace Retro {

public interface VariablesInterface : Object {
	public abstract weak Core core { get; set; }

	public abstract string? get_variable (string key);
	public abstract void set_variable (Variable[] variables);
	public abstract bool get_variable_update ();
}

}

