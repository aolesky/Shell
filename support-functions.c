
int compare_strings(char *str1, char *str2) {

	int i = 0;
	while (str1[i] != '\0' || str2[i] != '\0') {
		if (str1[i] != str2[i])
			return 0;

		i++;
	}

	return 1;
}

//Ensures the copied string null terminates if the initial string is too long
void copy_string(char from[], char to[], int length) {

	if (length == 0) {
		return;
	}
	int i = 0;
	while (i < length) {
		to[i] = from[i];
		if (from[i] == '\0')
			return;

		i++;
	}

	to[length - 1] = '\0';
}